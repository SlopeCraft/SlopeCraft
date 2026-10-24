project(SlopeCraft_VCLConfigLoader VERSION ${SlopeCraft_version} LANGUAGES CXX)

add_library(VCLConfigLoader STATIC
    VCLConfigLoader.h
    VCLConfigLoader.cpp)

target_link_libraries(VCLConfigLoader PRIVATE
    nlohmann_json::nlohmann_json
)

target_compile_features(VCLConfigLoader PUBLIC cxx_std_17)
target_include_directories(VCLConfigLoader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})