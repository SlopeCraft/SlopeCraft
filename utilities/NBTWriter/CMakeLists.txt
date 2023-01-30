set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

find_package(ZLIB 1.2.11 REQUIRED)

# if(ZLIB_FOUND)
# include_directories(ZLIB_INCLUDE_DIR)
# endif()
add_library(NBTWriter

    NBTWriter.h
    NBTWriter.cpp)

target_link_libraries(NBTWriter PUBLIC z)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(NBTWriter PUBLIC "-fvisibility=hidden" "-fPIC")
endif()

# target_compile_options(NBTWriter BEFORE PUBLIC -std=c++17)
