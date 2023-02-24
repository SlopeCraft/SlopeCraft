cmake_minimum_required(VERSION 3.14)

include(FetchContent)

set(SC_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

set(CMAKE_CXX_FLAGS "-fPIC")

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG "9.1.0"
  OVERRIDE_FIND_PACKAGE

  # QUIET false
  # FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/3rdParty
  # FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS FIND_PACKAGE_ARGS find_fmt_args
)

if(NOT ${fmt_POPULATED})
  message(STATUS "Downaloding and building fmtlib......")
endif()

FetchContent_MakeAvailable(fmt)

set(CMAKE_CXX_FLAGS ${SC_CMAKE_CXX_FLAGS})
unset(SC_CMAKE_CXX_FLAGS)

# message(STATUS "fmt_POPULATED = " )

# message(STATUS "find_fmt_args = " ${find_fmt_args})
