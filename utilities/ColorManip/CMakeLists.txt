project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# include_directories(../SlopeCraftL)
find_package(OpenMP REQUIRED)

add_library(ColorManip

    ColorManip.h
    ColorCvt.cpp
    ColorDiff.cpp
    CIEDE00.cpp

    newColorSet.hpp
    newTokiColor.hpp
    StaticMembers.cpp)

target_include_directories(ColorManip PUBLIC
    ${SlopeCraft_Eigen3_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities)

target_include_directories(ColorManip PRIVATE
    ${SlopeCraft_HeuristicFlow_include_dir})

target_link_libraries(ColorManip PUBLIC OpenMP::OpenMP_CXX)

# target_compile_options(ColorManip BEFORE PUBLIC "-std=c++17")
target_compile_options(ColorManip PRIVATE ${SlopeCraft_vectorize_flags})
target_compile_features(ColorManip PRIVATE cxx_std_20)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(ColorManip PUBLIC "-fPIC")
endif()