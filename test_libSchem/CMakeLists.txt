set(CMAKE_CXX_STANDARD 17)
include_directories(${SlopeCraft_Eigen3_include_dir})
include_directories(${CMAKE_SOURCE_DIR}/utilities)

add_executable(test_libSchem
    
    test_libSchem.cpp)

target_link_libraries(test_libSchem PRIVATE Schem NBTWriter -lz)