include(${CMAKE_CURRENT_LIST_DIR}/../required_deps/fmtlib.cmake)

find_package(Vulkan REQUIRED)

set(KOMPUTE_OPT_USE_BUILT_IN_FMT OFF)
set(KOMPUTE_OPT_USE_BUILT_IN_GOOGLE_TEST OFF)
set(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER OFF)
FetchContent_Declare(kompute
    GIT_REPOSITORY https://github.com/SlopeCraft/kompute
    GIT_TAG 37afc4f814eb941adb5c0c9a76fc0d8c4ebc6e53
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

message(STATUS "Downaloding SlopeCraft/kompute......")
FetchContent_MakeAvailable(kompute)

find_package(kompute REQUIRED)