project(Schem VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir})

add_library(Schem 
     
    Schem.h 

    SChem.cpp 
    bit_shrink.h 
    bit_shrink.cpp)


target_compile_options(Schem BEFORE PUBLIC -std=c++17)

target_link_libraries(Schem PRIVATE NBTWriter)