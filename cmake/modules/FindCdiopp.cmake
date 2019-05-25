#.rst:
# FindCdio
# --------
# Finds the cdio++ library
#
# This will define the following variables::
#
# CDIOPP_INCLUDE_DIRS - the cdio++ include directory
# CDIOPP_LIBRARIES - the cdio++ libraries
#
# and the following imported targets::
#
#   CDIOPP::CDIOPP - The cdio++ library

if(ENABLE_INTERNAL_CDIO)
  include(ExternalProject)
  file(STRINGS ${CMAKE_SOURCE_DIR}/tools/depends/target/libcdio-gplv3/Makefile VERSION)
  string(REGEX REPLACE ".*VERSION=([0-9.]*).*" "\\1" CDIOPP_VERSION "${VERSION}")

  # allow user to override the download URL with a local tarball
  # needed for offline build envs
  if(CDIO_URL)
    get_filename_component(CDIO_URL "${CDIO_URL}" ABSOLUTE)
  else()
    set(CDIO_URL http://mirrors.kodi.tv/build-deps/sources/libcdio-${CDIOPP_VERSION}.tar.gz)
  endif()
  if(VERBOSE)
    message(STATUS "CDIO_URL: ${CDIO_URL}")
  endif()

  set(CDIOPP_LIBRARY ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/lib/libcdio++.a)
  set(CDIOPP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/include)

  set(ISO9660PP_LIBRARY ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/lib/libiso9660++.a)
  set(ISO9660PP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/include)

  # todo: remove after converted all code to use c++ libs

  set(CDIO_LIBRARY ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/lib/libcdio.a)
  set(CDIO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/include)

  set(ISO9660_LIBRARY ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/lib/libiso9660.a)
  set(ISO9660_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/include)

  # end todo

  externalproject_add(Cdiopp
                      URL ${CDIO_URL}
                      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}/download
                      PREFIX ${CORE_BUILD_DIR}/libcdio
                      CONFIGURE_COMMAND ./configure --prefix ${CMAKE_BINARY_DIR}/${CORE_BUILD_DIR}
                                        --with-cd-drive=no --with-cd-info=no --with-cdda-player=no
                                        --with-cd-read=no --with-iso-info=no --with-iso-read=no
                                        --disable-example-progs --disable-cpp-progs --enable-cxx --disable-shared
                      BUILD_BYPRODUCTS ${CDIOPP_LIBRARY} ${ISO9660PP_LIBRARY} ${CDIO_LIBRARY} ${ISO9660_LIBRARY}
                      BUILD_IN_SOURCE 1)

  set_target_properties(Cdiopp PROPERTIES FOLDER "External Projects")

  set(CDIOPP_LIBRARIES ${CDIOPP_LIBRARY} ${ISO9660PP_LIBRARY} ${CDIO_LIBRARY} ${ISO9660_LIBRARY})
  set(CDIOPP_INCLUDE_DIRS ${CDIOPP_INCLUDE_DIR} ${ISO9660PP_INCLUDE_DIR} ${CDIO_INCLUDE_DIR} ${ISO9660_INCLUDE_DIR})
else()

  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_CDIOPP libcdio++>=0.94 QUIET)
    pkg_check_modules(PC_ISO9660PP libiso9660++>=0.94 QUIET)
  endif()

  find_path(CDIOPP_INCLUDE_DIR NAMES cdio++/cdio.hpp
                               PATHS ${PC_CDIOPP_libcdio++_INCLUDEDIR})

  find_library(CDIOPP_LIBRARY NAMES libcdio++ cdio++
                              PATHS ${PC_CDIOPP_libcdio++_LIBDIR})

  find_path(ISO9660PP_INCLUDE_DIR NAMES cdio++/iso9660.hpp
                                  PATHS ${PC_CDIOPP_libiso9660++_INCLUDEDIR})

  find_library(ISO9660PP_LIBRARY NAMES libiso9660++ iso9660++
                                 PATHS ${PC_CDIOPP_libiso9660++_LIBDIR})

  # todo: remove after converted all code to use c++ libs

  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_CDIO libcdio>=0.94 QUIET)
    pkg_check_modules(PC_ISO9660 libiso9660>=0.94 QUIET)
  endif()

  find_path(CDIO_INCLUDE_DIR NAMES cdio/cdio.h
                               PATHS ${PC_CDIO_libcdio_INCLUDEDIR})

  find_library(CDIO_LIBRARY NAMES libcdio cdio
                              PATHS ${PC_CDIO_libcdio_LIBDIR})

  find_path(ISO9660_INCLUDE_DIR NAMES cdio/iso9660.h
                                  PATHS ${PC_CDIO_libiso9660_INCLUDEDIR})

  find_library(ISO9660_LIBRARY NAMES libiso9660 iso9660
                                 PATHS ${PC_CDIO_libiso9660_LIBDIR})

  # end todo

  set(CDIOPP_VERSION ${PC_CDIOPP_libcdio++_VERSION})

  if(PC_CDIOPP_FOUND)
    set(CDIOPP_LIBRARIES ${CDIOPP_LIBRARY} ${ISO9660PP_LIBRARY} ${CDIO_LIBRARY} ${ISO9660_LIBRARY})
    set(CDIOPP_INCLUDE_DIRS ${CDIOPP_INCLUDE_DIR} ${ISO9660PP_INCLUDE_DIR} ${CDIO_INCLUDE_DIR} ${ISO9660_INCLUDE_DIR})

    if(NOT TARGET CDIOPP::CDIOPP)
      add_library(CDIOPP::CDIOPP UNKNOWN IMPORTED)
      set_target_properties(CDIOPP::CDIOPP PROPERTIES
                                           IMPORTED_LOCATION "${CDIOPP_LIBRARIES}"
                                           INTERFACE_INCLUDE_DIRECTORIES "${CDIOPP_INCLUDE_DIRS}")
    endif()
  endif()

  mark_as_advanced(CDIOPP_INCLUDE_DIR CDIOPP_LIBRARY ISO9660PP_INCLUDE_DIR ISO9660PP_LIBRARY
                   CDIO_INCLUDE_DIR CDIO_LIBRARY ISO9660_INCLUDE_DIR ISO9660_LIBRARY)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cdiopp
                                  REQUIRED_VARS CDIOPP_LIBRARY CDIOPP_INCLUDE_DIR ISO9660PP_INCLUDE_DIR ISO9660PP_LIBRARY
                                                CDIO_LIBRARY CDIO_INCLUDE_DIR ISO9660_INCLUDE_DIR ISO9660_LIBRARY
                                  VERSION_VAR CDIOPP_VERSION)
