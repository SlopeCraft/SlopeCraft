cmake_minimum_required(VERSION 3.5)
project(SlopeCraft VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# manage the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install_app)
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
if(DEFINED Qt6_DIR)
    set(SlopeCraft_Qt_root_dir ${Qt6_DIR})
else()
    set(SlopeCraft_Qt_root_dir D:/Qt/6.1.0/gcc12-shared)
endif()
if(NOT DEFINED SlopeCraft_Qt_root_dir)
    message(FATAL_ERROR "Please set the variable \"SlopeCraft_Qt_root_dir\". Otherwise cmake can\'t find Qt6")
endif()

set(CMAKE_PREFIX_PATH ${SlopeCraft_Qt_root_dir})
set(QT_DIR ${SlopeCraft_Qt_root_dir}/lib/cmake/Qt6)
set(QT_QMAKE_EXECUTABLE ${SlopeCraft_Qt_root_dir}/bin/qmake.exe)

# The executable to generate .qm files from .qm files
set(SlopeCraft_Qt_lrelease_executable ${SlopeCraft_Qt_root_dir}/bin/lrelease)
message("Testing lrelease : ")
message("The executable of lrelease is : " ${SlopeCraft_Qt_lrelease_executable})
execute_process(COMMAND ${SlopeCraft_Qt_lrelease_executable} -version
    OUTPUT_VARIABLE SlopeCraft_output_variable)
message("The output is : " ${SlopeCraft_output_variable})

# The executable to update .ts files
set(SlopeCraft_Qt_lupdate_executable ${SlopeCraft_Qt_root_dir}/bin/lupdate)
message("Testing lupdate : ")
message("The executable of lupdate is : " ${SlopeCraft_Qt_lupdate_executable})
execute_process(COMMAND ${SlopeCraft_Qt_lupdate_executable} -version
    OUTPUT_VARIABLE SlopeCraft_output_variable)
message("The output is : " ${SlopeCraft_output_variable})

# If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated.
set(SlopeCraft_update_ts_files OFF)


add_subdirectory(imageCutter)
add_subdirectory(GAConverter)
add_subdirectory(SlopeCraftL)
add_subdirectory(BlockListManager)
add_subdirectory(SlopeCraftMain)
add_subdirectory(MapViewer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install icon for macOS
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # imageCutter.app
    file(GLOB imageCutter_Icon "imageCutter/others/imageCutterIconNew.icns")
    install(FILES ${imageCutter_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/imageCutter.app/Contents/Resources)
    # SlopeCraft.app
    file(GLOB SlopeCraft_Icon "SlopeCraftMain/others/SlopeCraftIconNew.icns")
    install(FILES ${SlopeCraft_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/Resources)
    # MapViewer.app
    file(GLOB MapViewer_Icon "MapViewer/others/MapViewer.icns")
    install(FILES ${MapViewer_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/MapViewer.app/Contents/Resources)
endif()

# Install Blocks
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Install for macOS
    # Install FixedBlocks.json, CustomBlocks.json and README.md
    file(GLOB SlopeCraft_install_jsons
    "Blocks/*.json"
    "Blocks/*.md")
    install(FILES ${SlopeCraft_install_jsons}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks)
    # Install all png files of fixedblocks
    file(GLOB SlopeCraft_install_png_fixedblocks
    "Blocks/FixedBlocks/*.png")
    install(FILES ${SlopeCraft_install_png_fixedblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks/FixedBlocks)
    # Install all png files of customblocks
    file(GLOB SlopeCraft_install_png_customblocks
    "Blocks/CustomBlocks/*.png")
    install(FILES ${SlopeCraft_install_png_customblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks/CustomBlocks)
else()
    # Install for Windows OS. Linux OS and Unix OS probably works too.
    # Install FixedBlocks.json, CustomBlocks.json and README.md
    file(GLOB SlopeCraft_install_jsons
    "Blocks/*.json"
    "Blocks/*.md")
    install(FILES ${SlopeCraft_install_jsons}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks)
    # Install all png files of fixedblocks
    file(GLOB SlopeCraft_install_png_fixedblocks
    "Blocks/FixedBlocks/*.png")
    install(FILES ${SlopeCraft_install_png_fixedblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/FixedBlocks)
    # Install all png files of customblocks
    file(GLOB SlopeCraft_install_png_customblocks
    "Blocks/CustomBlocks/*.png")
    install(FILES ${SlopeCraft_install_png_customblocks}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/CustomBlocks)
endif()
