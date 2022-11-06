##################################################################################
# STAGE 0 - base environment with first set of runtime dependencies
##################################################################################
ARG BUILD_TAG
ARG GIT_COMMIT

FROM centos:centos7 as lhmiscutil-base-env

RUN yum -y --enablerepo=extras install epel-release && \
    yum -y install https://repo.ius.io/ius-release-el7.rpm && \
    yum clean all

##################################################################################
# STAGE 1 - build tools and libraries needed to build lhmiscutil
##################################################################################
FROM lhmiscutil-base-env as lhmiscutil-build-env

# for compiling and unit testing
RUN yum -y install \
        cmake3 \
        gcc \
        gcc-c++ \
        gtest-devel \
        make \
        rpm-build && \
    yum clean all

ADD ./modules/lhscriptutil/scripts/refreshOrSetupLHDistYumRepo.sh /refreshOrSetupLHDistYumRepo.sh
RUN /refreshOrSetupLHDistYumRepo.sh

ENTRYPOINT [ "bash" ]

##################################################################################
# STAGE 2 - the lhmiscutil source and compiled binaries
##################################################################################
FROM lhmiscutil-build-env as lhmiscutil-build

ADD . /lhmiscutil
RUN cd /lhmiscutil && \
    mkdir ./build && \
    cd ./build && \
    cmake3 \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCPACK_PACKAGE_DIRECTORY=/lhdist \
    ../ && \
    make && \
    make test && \
    make package
RUN /refreshOrSetupLHDistYumRepo.sh

ENV BUILD_TAG=${BUILD_TAG}
LABEL build_tag="${BUILD_TAG}"
ENV GIT_COMMIT=${GIT_COMMIT}
LABEL git_commit="${GIT_COMMIT}"

##################################################################################
# STAGE 3 - the base image with additional built runtime dependencies, lhmiscutil 
#           binaries and test binaries needed for running integration tests
#           includes everything from build-env
##################################################################################
FROM lhmiscutil-build as lhmiscutil-test-env

RUN cd /lhmiscutil/build && \
    make install && \
    ldconfig

##################################################################################
# STAGE 4 - the base image with additional built runtime dependencies and 
#           lhmiscutil binaries includes nothing from build-env
##################################################################################
FROM lhmiscutil-base-env as lhmiscutil-main

COPY --from=lhmiscutil-build /etc/ /etc/
COPY --from=lhmiscutil-build /opt/ /opt/
COPY --from=lhmiscutil-build /usr/ /usr/
COPY --from=lhmiscutil-build /lhmiscutil/ /lhmiscutil/
RUN cd /lhmiscutil/build && \
    make install && \
    make package && \
    ldconfig && \
    cd / && \
    rm -rf /lhmiscutil

##################################################################################
# STAGE 5 - the base image with additional built runtime dependencies and 
#           lhmiscutil binaries includes nothing from build-env
##################################################################################
FROM lhmiscutil-base-env as lhmiscutil-dist-test

COPY --from=lhmiscutil-build /lhdist/ /lhdist/
RUN /refreshOrSetupLHDistYumRepo.sh
RUN yum -y repo-pkgs lhdistrepo install && \
    ldconfig && \
    yum clean all

FROM lhmiscutil-base-env as lhmiscutil-dist

COPY --from=lhmiscutil-main /lhdist/ /lhdist/