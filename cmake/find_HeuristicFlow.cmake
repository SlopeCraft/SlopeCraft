
include(FetchContent)

FetchContent_Declare(Heu
    GIT_REPOSITORY https://github.com/SlopeCraft/HeuristicFlow.git
    GIT_TAG v1.6.4.1
    OVERRIDE_FIND_PACKAGE)

message(STATUS "Downloading HeuristicFlow")
FetchContent_MakeAvailable(Heu)

find_package(Heu 1.6.4 REQUIRED)