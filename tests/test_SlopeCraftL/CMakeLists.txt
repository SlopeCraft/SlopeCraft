set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_SlopeCraftL 
    test_SlopeCraftL.cpp
)

target_link_libraries(test_SlopeCraftL PRIVATE SlopeCraftL)