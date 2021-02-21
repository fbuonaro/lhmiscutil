##################################################################################
# STAGE 0 - base environment with first set of runtime dependencies
##################################################################################
FROM centos:centos7 as lhutil-s0-base-env
LABEL lhutil-stage-base-env="yes"
LABEL lhutil-stage-build-env="no"
LABEL lhutil-stage-build="no"
LABEL lhutil-stage-test-env="no"
LABEL lhutil-stage-main="no"

RUN yum -y --enablerepo=extras install epel-release && \
    yum -y install https://repo.ius.io/ius-release-el7.rpm && \
    yum clean all

##################################################################################
# STAGE 1 - build tools and libraries needed to build lhutil
##################################################################################
FROM lhutil-s0-base-env as lhutil-s1-build-env
LABEL lhutil-stage-base-env="no"
LABEL lhutil-stage-build-env="yes"
LABEL lhutil-stage-build="no"
LABEL lhutil-stage-test-env="no"
LABEL lhutil-stage-main="no"

# for compiling and unit testing
RUN yum -y install \
        cmake3 \
        gcc \
        gcc-c++ \
        gtest-devel \
        make && \
    yum clean all

ENTRYPOINT [ "bash" ]

##################################################################################
# STAGE 2 - the lhutil source and compiled binaries
##################################################################################
FROM lhutil-s1-build-env as lhutil-s2-build
LABEL lhutil-stage-base-env="no"
LABEL lhutil-stage-build-env="no"
LABEL lhutil-stage-build="yes"
LABEL lhutil-stage-test-env="no"
LABEL lhutil-stage-main="no"

ADD . /lhutil
RUN cd /lhutil && \
    mkdir ./build && \
    cd ./build && \
    cmake3 \
        -DCMAKE_BUILD_TYPE=Release \
        ../ && \
    make && \
    make test

##################################################################################
# STAGE 3 - the base image with additional built runtime dependencies, lhutil 
#           binaries and test binaries needed for running integration tests
#           includes everything from build-env
##################################################################################
FROM lhutil-s2-build as lhutil-s3-test-env
LABEL lhutil-stage-base-env="no"
LABEL lhutil-stage-build-env="no"
LABEL lhutil-stage-build="no"
LABEL lhutil-stage-test-env="yes"
LABEL lhutil-stage-main="no"

RUN cd /lhutil/build && \
    make install-lhutil

##################################################################################
# STAGE 4 - the base image with additional built runtime dependencies and 
#           lhutil binaries includes nothing from build-env
##################################################################################
FROM lhutil-s0-base-env as lhutil-s4-main-env
LABEL lhutil-stage-base-env="no"
LABEL lhutil-stage-build-env="no"
LABEL lhutil-stage-build="no"
LABEL lhutil-stage-test-env="no"
LABEL lhutil-stage-main="yes"

COPY --from=lhutil-s2-build /usr/ /usr/
COPY --from=lhutil-s2-build /lhutil/ /lhutil/
RUN cd /lhutil/build && \
    make install-lhutil && \
    cd / && \
    rm -rf /lhutil
