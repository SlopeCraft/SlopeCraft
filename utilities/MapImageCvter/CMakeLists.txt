set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${CMAKE_SOURCE_DIR}/utilities
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
)

find_package(OpenMP REQUIRED)

add_library(MapImageCvter 
    STATIC     
    MapImageCvter.h 
    MapImageCvter.cpp)

target_link_libraries(MapImageCvter PUBLIC ColorManip GAConverter OpenMP::OpenMP_CXX)
target_compile_options(MapImageCvter PRIVATE ${SlopeCraft_vectorize_flags})