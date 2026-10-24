project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES C CXX)

find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(xsimd REQUIRED)
find_package(Boost CONFIG REQUIRED)

add_library(ColorManip

    ColorManip.h
    ColorCvt.cpp
    ColorDiff.cpp
    CIEDE00.cpp

    newColorSet.hpp
    newTokiColor.hpp

    colorset_maptical.hpp
    colorset_optical.hpp
    imageConvert.hpp
    newColorSet.hpp
    newTokiColor.hpp

    MapImageCvter.h
)

target_compile_features(ColorManip PUBLIC cxx_std_23)

target_include_directories(ColorManip PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities
)
target_include_directories(ColorManip PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(ColorManip PUBLIC
    OpenMP::OpenMP_CXX
    Eigen3::Eigen
    xsimd
)

# target_compile_options(ColorManip BEFORE PUBLIC "-std=c++17")
target_compile_options(ColorManip PRIVATE ${SlopeCraft_vectorize_flags})


# ColorManip links to GPUInterface
target_link_libraries(ColorManip PUBLIC GPUInterface)
target_include_directories(ColorManip INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ColorManip PRIVATE "${CMAKE_BINARY_DIR}/SlopeCraftL")


if (NOT ${SlopeCraft_GPU_API} STREQUAL "None")
    add_executable(test_init_program tests/test_init_program.cpp)
    target_link_libraries(test_init_program PRIVATE
        OpenMP::OpenMP_CXX
        ColorManip
        CLI11::CLI11
    )

    add_executable(test_algo tests/test_algo.cpp)
    target_link_libraries(test_algo PRIVATE
        OpenMP::OpenMP_CXX
        ColorManip
        CLI11::CLI11
    )

    target_include_directories(test_algo PRIVATE ${cli11_include_dir})

    foreach (_algo r R H X l L)
        add_test(NAME test_algo_${_algo}
            COMMAND test_algo --algo ${_algo} --platform-idx 0 --device-idx 0
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endforeach (_algo r R H X l L)
endif ()
