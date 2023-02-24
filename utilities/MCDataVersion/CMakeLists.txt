project(SlopeCraft_MCDataVersion LANGUAGES C CXX)

add_library(MCDataVersion STATIC
    MCDataVersion.h
    MCDataVersion.cpp
)

include(${CMAKE_SOURCE_DIR}/cmake/configure_magic_enum.cmake)

find_package(magic_enum REQUIRED)

target_link_libraries(MCDataVersion PUBLIC magic_enum::magic_enum)
target_compile_features(MCDataVersion PRIVATE cxx_std_20)

target_include_directories(MCDataVersion INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})