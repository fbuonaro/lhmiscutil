cmake_minimum_required( VERSION 3.10 )

project( liblhmiscutil 
         DESCRIPTION "lhmiscutil library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhmiscutil/liblhmiscutil"
         LANGUAGES CXX
         VERSION 1.0.0 )

##############################################################
# general
##############################################################

include( GNUInstallDirs )

set( CMAKE_CXX_STANDARD 11 )

add_compile_options( -Wall -Wextra -pedantic -Werror )

##############################################################
# library
##############################################################

# install directories
set( LH_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( LH_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhmiscutil_${PROJECT_VERSION}" )
set( LH_INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhmiscutil_${PROJECT_VERSION}" )

# library name
set( LH_LIB_NAME lhmiscutil )

# source files
set( LH_LIB_SRC_FILES 
     "src/singleton.cxx" )
# library dependencies
set( LH_LIB_PUBLIC_LINKLIBS )
set( LH_LIB_PRIVATE_LINKLIBS )
# header dependencies
set( LH_LIB_PUBLIC_INCLUDES 
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhmiscutil>        # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                                 # export
set( LH_LIB_PRIVATE_INCLUDES 
     "${CMAKE_CURRENT_BINARY_DIR}" )

lh_add_library()

##############################################################
# unit tests
##############################################################

# pull in gtest
find_package( GTest REQUIRED )

add_executable( testlhmiscutil
                "test/testlhmiscutil.cxx" )

target_link_libraries( testlhmiscutil
                       PRIVATE
                           "${GTEST_BOTH_LIBRARIES}"
                           lhmiscutil )

target_include_directories( testlhmiscutil
                            PRIVATE
                                "${CMAKE_CURRENT_BINARY_DIR}"
                                "${GTEST_INCLUDE_DIRS}" )

include( GoogleTest )
gtest_add_tests( TARGET testlhmiscutil )

##############################################################
# installation
##############################################################

# name of component to install
set(LH_COMPONENT_NAME liblhmiscutil )
# directories to install
set(LH_LIB_PUBLIC_INCLUDE_DIRS "include/lhmiscutil/lhmiscutil")

lh_add_install_library()

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${LH_INSTALL_INCDIR}")
set( PC_LIBDIR "${LH_INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS )
set( PC_PUBLIBS "-llhmiscutil" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

lh_add_pkgconfig()

##############################################################
# cmake packaging
##############################################################

lh_add_install_cmake_config()

##############################################################
# rpm packaging
##############################################################
 
# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_LIBLHMISCUTIL_PACKAGE_REQUIRES_POST "sed" PARENT_SCOPE) 
set( CPACK_RPM_LIBLHMISCUTIL_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" PARENT_SCOPE)

set(LH_PACKAGE_DESC "liblhmiscutil libraries package")

lh_add_cpack_component()
