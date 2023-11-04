include(${CMAKE_SOURCE_DIR}/cmake/configure_fmtlib.cmake)

find_package(Vulkan REQUIRED)

set(KOMPUTE_OPT_USE_BUILT_IN_FMT OFF)
set(KOMPUTE_OPT_USE_BUILT_IN_GOOGLE_TEST OFF)
set(KOMPUTE_OPT_USE_BUILT_IN_VULKAN_HEADER OFF)
FetchContent_Declare(kompute
    GIT_REPOSITORY https://github.com/SlopeCraft/kompute
    GIT_TAG 72459d97a9ddbe3651d8449b2f8476151a7cb25d
    OVERRIDE_FIND_PACKAGE)

message(STATUS "Downaloding SlopeCraft/kompute......")
FetchContent_MakeAvailable(kompute)

find_package(kompute REQUIRED)