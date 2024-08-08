 find_package(tl-expected 1.1.0 QUIET)
 if (NOT tl-expected_FOUND)
     set(EXPECTED_BUILD_TESTS OFF)
     include(FetchContent)
     message(STATUS "Downloading tl-expected...")
     FetchContent_Declare(tl-expected
         GIT_REPOSITORY https://github.com/TartanLlama/expected.git
         GIT_TAG v1.1.0
         OVERRIDE_FIND_PACKAGE
         EXCLUDE_FROM_ALL
     )
     FetchContent_MakeAvailable(tl-expected)
     find_package(tl-expected CONFIG REQUIRED)
 endif ()