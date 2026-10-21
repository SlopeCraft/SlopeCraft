set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_library(MapImageCvter
    STATIC
    MapImageCvter.h
    MapImageCvter.cpp)

target_include_directories(MapImageCvter PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities
)

target_link_libraries(MapImageCvter PUBLIC
    ColorManip
    OpenMP::OpenMP_CXX
)

target_include_directories(MapImageCvter PRIVATE "${CMAKE_BINARY_DIR}/SlopeCraftL")
target_compile_options(MapImageCvter PRIVATE ${SlopeCraft_vectorize_flags})

target_compile_features(MapImageCvter PUBLIC cxx_std_23)
