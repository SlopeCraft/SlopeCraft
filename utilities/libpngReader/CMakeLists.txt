add_library(libpng_reader STATIC
    libpng_reader.cpp
    libpng_reader.h)

find_package(PNG REQUIRED)

target_compile_features(libpng_reader PUBLIC cxx_std_23)
target_link_libraries(libpng_reader PUBLIC PNG::PNG)
target_include_directories(libpng_reader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)