cmake_minimum_required( VERSION 3.10 )

project( liblhutil )

# general flags for building successfully
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

# for debugging
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -Wall -Wextra -W" )
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -DDEBUG" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLHC_CXX_FLAGS_DBG}" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${LLHC_CXX_FLAGS_DBG}" )

##############################################################
# library
##############################################################

set( LIB_SRC_DIR "src" )
set( LIB_INC_DIR "include" )

# pull in gtest
find_package( GTest REQUIRED )
if( NOT GTEST_FOUND )
    message( FATAL_ERROR "gtest could not be found" )
endif( NOT GTEST_FOUND )

add_library( lhutil SHARED
    "${LIB_SRC_DIR}/singleton.cxx" )

# target_link_libraries( lhutil
#     PUBLIC
#     PRIVATE
#     )

target_include_directories( lhutil
    PUBLIC
        "${LIB_INC_DIR}/lhutil"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}" )

##############################################################
# unit tests
##############################################################

set( TST_SRC_DIR "./test" )

enable_testing()

add_executable( testlhutil
    "${TST_SRC_DIR}/testlhutil.cxx" )

target_link_libraries( testlhutil
    PRIVATE
        "${GTEST_BOTH_LIBRARIES}"
        lhutil )

target_include_directories( testlhutil
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${GTEST_INCLUDE_DIRS}"
        "${LIB_INC_DIR}/lhutil" )

include( GoogleTest )
gtest_add_tests( TARGET testlhutil )

##############################################################
# installation
##############################################################

# library
install( TARGETS lhutil
         LIBRARY
            DESTINATION lib
            COMPONENT lhutil )

# include directory
install( DIRECTORY "${LIB_INC_DIR}/lhutil/lhutil"
         DESTINATION include
         COMPONENT lhutil
         FILES_MATCHING PATTERN "*.h" )

ADD_CUSTOM_TARGET( install-lhutil
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=lhutil"
                   -P "cmake_install.cmake" )
