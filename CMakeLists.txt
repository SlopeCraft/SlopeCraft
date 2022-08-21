cmake_minimum_required(VERSION 3.5)
project(SlopeCraft VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# manage the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX})
#message(${CMAKE_INSTALL_PREFIX})

# manage the include dir of HeuristicFlow
set(SlopeCraft_HeuristicFlow_include_dir D:/CppLibs/HeuristicFlow-1.6.2.1)
if(NOT DEFINED SlopeCraft_HeuristicFlow_include_dir)
    message(FATAL_ERROR "Please set the vairable \"SlopeCraft_HeuristicFlow_include_dir\".")
endif()

# manage the include dir of Eigen
set(SlopeCraft_Eigen3_include_dir D:/CppLibs/libEigen3/include/eigen3)
if(NOT DEFINED SlopeCraft_Eigen3_include_dir)
    message(FATAL_ERROR "Please set the vairable \"SlopeCraft_Eigen3_include_dir\".")
endif()

# manage the directory of Qt
set(SlopeCraft_Qt_root_dir D:/Qt/6.1.0/gcc12-shared)
if(NOT DEFINED SlopeCraft_Qt_root_dir)
    message(FATAL_ERROR "Please set the vairable \"SlopeCraft_Qt_root_dir\".")
endif()

set(CMAKE_PREFIX_PATH ${SlopeCraft_Qt_root_dir})
set(QT_DIR ${SlopeCraft_Qt_root_dir}/lib/cmake/Qt6)
set(QT_QMAKE_EXECUTABLE ${SlopeCraft_Qt_root_dir}/bin/qmake.exe)

add_subdirectory(GAConverter)
add_subdirectory(SlopeCraftL)
add_subdirectory(imageCutter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

install(TARGETS SlopeCraftL imageCutter
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/please_delete_this_folder
)