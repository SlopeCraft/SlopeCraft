
include(${CMAKE_SOURCE_DIR}/cmake/configure_cli11.cmake)

# test block state and resource parsing
add_executable(test_VCL_blockstate tests/test_VCL_blockstate.cpp)
target_link_libraries(test_VCL_blockstate PRIVATE VisualCraftL)
target_include_directories(test_VCL_blockstate PRIVATE ${cli11_include_dir})

# convert json from SCL to VCL
add_executable(SCL_json_cvt tests/SCL_json_cvt.cpp)
target_include_directories(SCL_json_cvt PRIVATE
    ${SlopeCraft_Nlohmann_json_include_dir})

# test block class properties
add_executable(test_block_class tests/test_block_class.cpp)
target_link_libraries(test_block_class PRIVATE VisualCraftL)

# sort VCL json
add_executable(VCL_json_sort tests/VCL_json_sort.cpp)
target_link_libraries(VCL_json_sort PRIVATE VisualCraftL)
target_include_directories(VCL_json_sort PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir})

# test computing projection images
add_executable(test_VCL_project_image tests/test_VCL_project_image.cpp)
target_link_libraries(test_VCL_project_image PRIVATE VisualCraftL PNG::PNG)
target_include_directories(test_VCL_project_image PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_project_image)

# test showing model
add_executable(test_VCL_model tests/test_VCL_model.cpp)
target_link_libraries(test_VCL_model PRIVATE VisualCraftL)
target_include_directories(test_VCL_model PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir})

# internal test for rotation
add_executable(itest_VCL_rotate tests/itest_VCL_rotate.cpp)
target_link_libraries(itest_VCL_rotate PRIVATE VisualCraftL_static)
target_include_directories(itest_VCL_rotate PRIVATE
    ${cli11_include_dir})

add_test(NAME test_rotate COMMAND itest_VCL_rotate WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_images.cmake)

add_test(NAME test_block_class COMMAND test_block_class WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# automatic tests
include(generate_tests.cmake)

add_executable(test_VCL_version tests/test_VCL_version.cpp)
target_link_libraries(test_VCL_version PRIVATE VisualCraftL)
target_compile_features(test_VCL_version PRIVATE cxx_std_17)

add_test(NAME test_VCL_version COMMAND test_VCL_version WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})