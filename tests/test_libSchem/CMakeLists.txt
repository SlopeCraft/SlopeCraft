set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_SOURCE_DIR}/utilities)

add_executable(test_libSchem

    test_libSchem.cpp)

target_link_libraries(test_libSchem PRIVATE Schem NBTWriter -lz)

add_test(NAME test_libSchem
    COMMAND test_libSchem)