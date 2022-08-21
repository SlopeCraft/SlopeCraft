cmake_minimum_required(VERSION 3.5)
project(SlopeCraft VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# manage the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX})


# manage the include dir of HeuristicFlow
set(SlopeCraft_HeuristicFlow_include_dir D:/CppLibs/HeuristicFlow-1.6.2.1)
if(NOT DEFINED SlopeCraft_HeuristicFlow_include_dir)
    message(FATAL_ERROR "Please set the variable \"SlopeCraft_HeuristicFlow_include_dir\". Otherwise cmake can\'t find HeuristicFlow")
endif()

# manage the include dir of Eigen
set(SlopeCraft_Eigen3_include_dir D:/CppLibs/libEigen3/include/eigen3)
if(NOT DEFINED SlopeCraft_Eigen3_include_dir)
    message(FATAL_ERROR "Please set the variable \"SlopeCraft_Eigen3_include_dir\". Otherwise cmake can\'t find Eigen")
endif()

# manage the directory of Qt
set(SlopeCraft_Qt_root_dir D:/Qt/6.1.0/gcc12-shared)
if(NOT DEFINED SlopeCraft_Qt_root_dir)
    message(FATAL_ERROR "Please set the variable \"SlopeCraft_Qt_root_dir\". Otherwise cmake can\'t find Qt6")
endif()

set(CMAKE_PREFIX_PATH ${SlopeCraft_Qt_root_dir})
set(QT_DIR ${SlopeCraft_Qt_root_dir}/lib/cmake/Qt6)
set(QT_QMAKE_EXECUTABLE ${SlopeCraft_Qt_root_dir}/bin/qmake.exe)

add_subdirectory(imageCutter)
add_subdirectory(GAConverter)
add_subdirectory(SlopeCraftL)
add_subdirectory(BlockListManager)
add_subdirectory(SlopeCraftMain)
#add_subdirectory(MapViewer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# install FixedBlocks.json, CustomBlocks.json and README.md
file(GLOB SlopeCraft_install_jsons
    "Blocks/*.json"
    "Blocks/*.md")
install(FILES ${SlopeCraft_install_jsons}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks)

# install all png files of fixedblocks
file(GLOB SlopeCraft_install_png_fixedblocks
    "Blocks/FixedBlocks/*.png")
install(FILES ${SlopeCraft_install_png_fixedblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/FixedBlocks)

# install all png files of customblocks
file(GLOB SlopeCraft_install_png_customblocks
    "Blocks/CustomBlocks/*.png")
install(FILES ${SlopeCraft_install_png_customblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/CustomBlocks)


