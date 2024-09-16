add_library(version_set INTERFACE)
target_sources(version_set INTERFACE version_set.hpp)
target_include_directories(version_set INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})