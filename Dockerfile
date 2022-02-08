##################################################################################
# STAGE 0 - base environment with first set of runtime dependencies
##################################################################################
FROM centos:centos7 as lhmiscutil-s0-base-env
LABEL lhmiscutil-stage-base-env="yes"
LABEL lhmiscutil-stage-build-env="no"
LABEL lhmiscutil-stage-build="no"
LABEL lhmiscutil-stage-test-env="no"
LABEL lhmiscutil-stage-main="no"

RUN yum -y --enablerepo=extras install epel-release && \
    yum -y install https://repo.ius.io/ius-release-el7.rpm && \
    yum clean all

##################################################################################
# STAGE 1 - build tools and libraries needed to build lhmiscutil
##################################################################################
FROM lhmiscutil-s0-base-env as lhmiscutil-s1-build-env
LABEL lhmiscutil-stage-base-env="no"
LABEL lhmiscutil-stage-build-env="yes"
LABEL lhmiscutil-stage-build="no"
LABEL lhmiscutil-stage-test-env="no"
LABEL lhmiscutil-stage-main="no"

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
# STAGE 2 - the lhmiscutil source and compiled binaries
##################################################################################
FROM lhmiscutil-s1-build-env as lhmiscutil-s2-build
LABEL lhmiscutil-stage-base-env="no"
LABEL lhmiscutil-stage-build-env="no"
LABEL lhmiscutil-stage-build="yes"
LABEL lhmiscutil-stage-test-env="no"
LABEL lhmiscutil-stage-main="no"

ADD . /lhmiscutil
RUN cd /lhmiscutil && \
    mkdir ./build && \
    cd ./build && \
    cmake3 \
        -DCMAKE_BUILD_TYPE=Release \
        ../ && \
    make && \
    make test

##################################################################################
# STAGE 3 - the base image with additional built runtime dependencies, lhmiscutil 
#           binaries and test binaries needed for running integration tests
#           includes everything from build-env
##################################################################################
FROM lhmiscutil-s2-build as lhmiscutil-s3-test-env
LABEL lhmiscutil-stage-base-env="no"
LABEL lhmiscutil-stage-build-env="no"
LABEL lhmiscutil-stage-build="no"
LABEL lhmiscutil-stage-test-env="yes"
LABEL lhmiscutil-stage-main="no"

RUN cd /lhmiscutil/build && \
    make install-lhmiscutil

##################################################################################
# STAGE 4 - the base image with additional built runtime dependencies and 
#           lhmiscutil binaries includes nothing from build-env
##################################################################################
FROM lhmiscutil-s0-base-env as lhmiscutil-s4-main-env
LABEL lhmiscutil-stage-base-env="no"
LABEL lhmiscutil-stage-build-env="no"
LABEL lhmiscutil-stage-build="no"
LABEL lhmiscutil-stage-test-env="no"
LABEL lhmiscutil-stage-main="yes"

COPY --from=lhmiscutil-s2-build /usr/ /usr/
COPY --from=lhmiscutil-s2-build /lhmiscutil/ /lhmiscutil/
RUN cd /lhmiscutil/build && \
    make install-lhmiscutil && \
    cd / && \
    rm -rf /lhmiscutil
