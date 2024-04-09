project(SlopeCraft_MCDataVersion LANGUAGES C CXX)

add_library(MCDataVersion STATIC
    MCDataVersion.h
    MCDataVersion.cpp
)

find_package(magic_enum REQUIRED)

target_link_libraries(MCDataVersion PUBLIC magic_enum::magic_enum)
target_compile_features(MCDataVersion PRIVATE cxx_std_20)

target_include_directories(MCDataVersion INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(MCDataVersion PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif ()