set(SOURCES "")
set(HEADERS "")

if(SDBUSPP_FOUND)
  list(APPEND SOURCES OSScreenSaverFreedesktop.cpp)
  list(APPEND HEADERS OSScreenSaverFreedesktop.h)
endif()

if(SOURCES)
  core_add_library(windowing_linux)
endif()
