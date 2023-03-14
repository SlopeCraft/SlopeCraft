project(SlopeCraft_VCLConfigLoader VERSION ${SlopeCraft_version} LANGUAGES CXX)

add_library(VCLConfigLoader STATIC
    VCLConfigLoader.h
    VCLConfigLoader.cpp)

target_include_directories(VCLConfigLoader PRIVATE ${SlopeCraft_Nlohmann_json_include_dir})
target_compile_features(VCLConfigLoader PUBLIC cxx_std_17)
target_include_directories(VCLConfigLoader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})