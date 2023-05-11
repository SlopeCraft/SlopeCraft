# https://github.com/mateidavid/zstr

include(FetchContent)

FetchContent_Declare(zstr
    GIT_REPOSITORY
    https://github.com/mateidavid/zstr.git GIT_TAG v1.0.7
    OVERRIDE_FIND_PACKAGE)

FetchContent_MakeAvailable(zstr)