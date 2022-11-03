project(SlopeCraft_libSCGUI VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(AbstractGUI STATIC libAbstractGUI.h libAbstractGUI.cpp)