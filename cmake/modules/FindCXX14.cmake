include(TestCXXAcceptsFlag)

# try to use compiler flag -std=c++14
check_cxx_accepts_flag("-std=c++14" CXX_FLAG_CXX14)
if(CXX_FLAG_CXX14)
  add_options (CXX ALL_BUILDS "-std=c++14")
  set(CXX_STD14_FLAGS "-std=c++14")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CXX14 DEFAULT_MSG CXX_STD14_FLAGS)
