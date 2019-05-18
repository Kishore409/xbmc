set(SOURCES SSE4.cpp)
set(HEADERS SSE4.h)

core_add_library(sse4)
target_compile_options(sse4 PRIVATE -msse4.1)
