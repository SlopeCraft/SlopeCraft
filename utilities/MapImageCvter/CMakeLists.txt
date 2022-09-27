set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/utilities
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
)

add_library(MapImageCvter 
    STATIC     
    MapImageCvter.h 
    MapImageCvter.cpp)

target_link_libraries(MapImageCvter PUBLIC ColorManip GAConverter)