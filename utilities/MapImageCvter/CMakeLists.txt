set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(cereal REQUIRED)

add_library(MapImageCvter
    STATIC
    MapImageCvter.h
    MapImageCvter.cpp)

target_include_directories(MapImageCvter PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities
    ${SlopeCraft_HeuristicFlow_include_dir}
)

target_link_libraries(MapImageCvter PUBLIC
    ColorManip
    GAConverter
    OpenMP::OpenMP_CXX
    cereal::cereal)

target_include_directories(MapImageCvter PRIVATE "${CMAKE_BINARY_DIR}/SlopeCraftL")
target_compile_options(MapImageCvter PRIVATE ${SlopeCraft_vectorize_flags})

target_compile_features(MapImageCvter PUBLIC cxx_std_23)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(MapImageCvter PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif ()