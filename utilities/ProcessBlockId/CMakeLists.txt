cmake_minimum_required(VERSION 3.20)
project(SlopeCraft_ProcessBlockId LANGUAGES CXX)

add_library(ProcessBlockId STATIC
    process_block_id.h
    process_block_id.cpp)

target_compile_features(ProcessBlockId PUBLIC cxx_std_20)
target_include_directories(ProcessBlockId INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_process_block_id
    test_process_blk_id.cpp)

target_link_libraries(test_process_block_id PRIVATE ProcessBlockId)
