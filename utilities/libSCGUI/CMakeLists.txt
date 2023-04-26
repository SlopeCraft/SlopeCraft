project(SlopeCraft_libSCGUI VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/utilities/libAbstractGUI
    ${CMAKE_SOURCE_DIR}/SlopeCraftL)

add_library(SCGUI STATIC libSCGUI.h libSCGUI.cpp)

target_link_libraries(SCGUI PRIVATE AbstractGUI SlopeCraftL)