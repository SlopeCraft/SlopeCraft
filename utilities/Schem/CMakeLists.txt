project(Schem VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)

# include_directories(${SlopeCraft_Eigen3_include_dir})
add_library(Schem

    Schem.h

    Schem.cpp
    bit_shrink.h
    bit_shrink.cpp)

target_include_directories(Schem PUBLIC ${SlopeCraft_Eigen3_include_dir})

# target_compile_options(Schem BEFORE PUBLIC -std=c++17)
target_link_libraries(Schem PUBLIC NBTWriter)
target_compile_features(Schem PRIVATE cxx_std_20)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(Schem PUBLIC "-fvisibility=hidden")
    target_compile_options(Schem PRIVATE "-fPIC")
endif()