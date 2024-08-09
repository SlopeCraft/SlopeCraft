cmake_minimum_required(VERSION 3.29)

find_package(ZLIB REQUIRED)

find_package(libnbt++)

if (libnbt++_FOUND)
    message(STATUS "Found installed libnbt++")
    return()
endif ()

message(STATUS "Failed to find libnbt++, cloning...")
set(NBT_BUILD_SHARED OFF CACHE BOOL "")
set(NBT_BUILD_TESTS OFF CACHE BOOL "")
FetchContent_Declare(libnbt++
    GIT_REPOSITORY https://github.com/PrismLauncher/libnbtplusplus
    GIT_TAG 23b955121b8217c1c348a9ed2483167a6f3ff4ad #Merge pull request #3 from TheKodeToad/max-depth-attempt2
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL)
message(STATUS "Cloning libnbt++...")
FetchContent_MakeAvailable(libnbt++)
find_package(libnbt++ REQUIRED)

target_link_libraries(nbt++ ZLIB::ZLIB)
target_compile_features(nbt++ PUBLIC cxx_std_20)
if (${MSVC})
    target_compile_options(nbt++ PRIVATE /std:c++20)
endif ()