cmake_minimum_required( VERSION 3.10 )

project( liblhmiscutil )

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

add_library( lhmiscutil SHARED
    "${LIB_SRC_DIR}/singleton.cxx" )

# target_link_libraries( lhmiscutil
#     PUBLIC
#     PRIVATE
#     )

target_include_directories( lhmiscutil
    PUBLIC
        "${LIB_INC_DIR}/lhmiscutil"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}" )

##############################################################
# unit tests
##############################################################

set( TST_SRC_DIR "./test" )

enable_testing()

add_executable( testlhmiscutil
    "${TST_SRC_DIR}/testlhmiscutil.cxx" )

target_link_libraries( testlhmiscutil
    PRIVATE
        "${GTEST_BOTH_LIBRARIES}"
        lhmiscutil )

target_include_directories( testlhmiscutil
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${GTEST_INCLUDE_DIRS}"
        "${LIB_INC_DIR}/lhmiscutil" )

include( GoogleTest )
gtest_add_tests( TARGET testlhmiscutil )

##############################################################
# installation
##############################################################

# library
install( TARGETS lhmiscutil
         LIBRARY
            DESTINATION lib
            COMPONENT lhmiscutil )

# include directory
install( DIRECTORY "${LIB_INC_DIR}/lhmiscutil/lhmiscutil"
         DESTINATION include
         COMPONENT lhmiscutil
         FILES_MATCHING PATTERN "*.h" )

ADD_CUSTOM_TARGET( install-lhmiscutil
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=lhmiscutil"
                   -P "cmake_install.cmake" )
