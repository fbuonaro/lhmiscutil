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
set( INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhmiscutil_${PROJECT_VERSION}" )
set( INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhmiscutil_${PROJECT_VERSION}" )

# source files
set( LIBLHMISCUTIL_SRC_FILES 
     "src/singleton.cxx" )
# library dependencies
set( LIBLHMISCUTIL_PUBLIC_LINKLIBS )
set( LIBLHMISCUTIL_PRIVATE_LINKLIBS )
# header dependencies
set( LIBLHMISCUTIL_PUBLIC_INCLUDES 
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhmiscutil>        # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                                 # export
set( LIBLHMISCUTIL_PRIVATE_INCLUDES 
     "${CMAKE_CURRENT_BINARY_DIR}" )

# boilerplate ..
# libs
add_library( lhmiscutil SHARED ${LIBLHMISCUTIL_SRC_FILES} )
add_library( lhmiscutil_static STATIC ${LIBLHMISCUTIL_SRC_FILES} )
# lib deps
target_link_libraries( lhmiscutil
                       PUBLIC "${LIBLHMISCUTIL_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIBLHMISCUTIL_PRIVATE_LINKLIBS}" )
target_link_libraries( lhmiscutil_static
                       PUBLIC "${LIBLHMISCUTIL_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIBLHMISCUTIL_PRIVATE_LINKLIBS}" )
# header deps
target_include_directories( lhmiscutil
                            PUBLIC "${LIBLHMISCUTIL_PUBLIC_INCLUDES}"
                            PRIVATE "${LIBLHMISCUTIL_PRIVATE_INCLUDES}" )
target_include_directories( lhmiscutil_static
                            PUBLIC "${LIBLHMISCUTIL_PUBLIC_INCLUDES}"
                            PRIVATE "${LIBLHMISCUTIL_PRIVATE_INCLUDES}" )
# properties
set_target_properties( lhmiscutil
                       PROPERTIES 
                           LIBRARY_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           SOVERSION "${PROJECT_VERSION}"
                           VERSION "${PROJECT_VERSION}"
                           CLEAN_DIRECT_OUTPUT 1)
set_target_properties( lhmiscutil_static 
                       PROPERTIES
                           ARCHIVE_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           OUTPUT_NAME lhmiscutil )
# .. boilerplate

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

# boilerplate ..
# install lib/bin
install( TARGETS lhmiscutil lhmiscutil_static
         EXPORT liblhmiscutil-targets 
         COMPONENT liblhmiscutil
         RUNTIME DESTINATION "${INSTALL_BINDIR}"
         COMPONENT liblhmiscutil
         LIBRARY DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhmiscutil
         ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhmiscutil
         INCLUDES DESTINATION "${LIBLHMISCUTIL_PUBLIC_INCLUDES}" )
# install header dir
install( DIRECTORY "include/lhmiscutil/lhmiscutil"
         DESTINATION "${INSTALL_INCDIR}"
         COMPONENT liblhmiscutil
         FILES_MATCHING PATTERN "*.h" )
# install export for things outside of build
install( EXPORT liblhmiscutil-targets 
         FILE liblhmiscutilTargets.cmake 
         NAMESPACE liblhmiscutil:: 
         DESTINATION "${INSTALL_LIBDIR}/cmake"
         COMPONENT liblhmiscutil )
# export export for things in build
export( EXPORT liblhmiscutil-targets
    FILE ${CMAKE_BINARY_DIR}/cmake/liblhmiscutilTargets.cmake 
    NAMESPACE liblhmiscutil:: )
ADD_CUSTOM_TARGET( install-lhmiscutil
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=liblhmiscutil"
                   -P "cmake_install.cmake" )
# .. boilerplate

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${INSTALL_INCDIR}")
set( PC_LIBDIR "${INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS )
set( PC_PUBLIBS "-llhmiscutil" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

# boilerplate ..
# configure pc file
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/component.pc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
                @ONLY )
# install pc file
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
         DESTINATION "${PC_INSTALL_DIR}"
         COMPONENT liblhmiscutil )
# configure pc relocation script
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/rpm_relocate_pc.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh"
                @ONLY )
# .. boilerplate

##############################################################
# cmake packaging
##############################################################

# boilerplate ..
include(CMakePackageConfigHelpers)

configure_package_config_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                               INSTALL_DESTINATION "${INSTALL_LIBDIR}/cmake" )

write_basic_package_version_file( "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                                  VERSION "${PROJECT_VERSION}"
                                  COMPATIBILITY SameMajorVersion )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${INSTALL_LIBDIR}/cmake"
         COMPONENT liblhmiscutil )
# .. boilerplate

##############################################################
# rpm packaging
##############################################################
 
# boilerplate ..
set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
set( CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
set( CPACK_GENERATOR "RPM" )

# set( CPACK_GENERATOR "RPM" "TGZ" )
# CPACK_RPM_BUILD_REQUIRES
# CPACK_RPM_liblhmiscutil_REQUIRES # component requires

set( CPACK_RPM_PACKAGE_RELOCATABLE ON)
set( CPACK_RPM_COMPONENT_INSTALL ON )

# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_PACKAGE_REQUIRES_POST "sed" ) 
set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" )

include( CPack )

# component, group can be different; group will appended to package
cpack_add_component(liblhmiscutil
    DISPLAY_NAME  "lhmiscutil library"
    DESCRIPTION   "lhmiscutil library"
    GROUP liblhmiscutil)
cpack_add_component_group(liblhmiscutil)
# .. boilerplate