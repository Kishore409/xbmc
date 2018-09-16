#.rst:
# FindSDBUSPP
# -------
# Finds the sdbuspp library
#
# This will define the following variables::
#
# SDBUSPP_FOUND        - system has SDBUSPP
# SDBUSPP_INCLUDE_DIRS - the SDBUSPP include directory
# SDBUSPP_LIBRARIES    - the SDBUSPP libraries
########### SDBUSPP_DEFINITIONS  - the SDBUSPP definitions
#

if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_SDBUSPP sdbus-c++ QUIET)
endif()

find_path(SDBUSPP_INCLUDE_DIR NAMES sdbus-c++.h
                           PATH_SUFFIXES sdbus-c++
                           PATHS ${PC_SDBUSPP_INCLUDE_DIR})
find_library(SDBUSPP_LIBRARY NAMES sdbus-c++
                          PATHS ${PC_SDBUSPP_LIBDIR})

set(SDBUSPP_VERSION ${PC_SDBUSPP_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDBuspp
                                  REQUIRED_VARS SDBUSPP_LIBRARY SDBUSPP_INCLUDE_DIR
                                  VERSION_VAR SDBUSPP_VERSION)

if(SDBUSPP_FOUND)
  set(SDBUSPP_LIBRARIES ${SDBUSPP_LIBRARY})
  set(SDBUSPP_INCLUDE_DIRS ${SDBUSPP_INCLUDE_DIR} ${SDBUSPP_ARCH_INCLUDE_DIR})
  set(SDBUSPP_DEFINITIONS -DHAS_SDBUSPP=1)
endif()

mark_as_advanced(SDBUSPP_INCLUDE_DIR SDBUSPP_LIBRARY)
