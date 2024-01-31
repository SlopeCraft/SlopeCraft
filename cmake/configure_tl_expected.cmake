 find_package(tl-expected QUIET)
 if (NOT tl-expected_FOUND)
     include(FetchContent)
     message(STATUS "Downloading tl-expected...")
     FetchContent_Declare(tl-expected
         GIT_REPOSITORY https://github.com/TartanLlama/expected.git
         GIT_TAG v1.1
         OVERRIDE_FIND_PACKAGE)
     FetchContent_MakeAvailable(tl-expected)
     find_package(tl-expected CONFIG REQUIRED)
 endif ()