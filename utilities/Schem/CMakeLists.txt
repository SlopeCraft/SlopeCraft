project(Schem VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
add_library(Schem

    Schem.h

    Schem.cpp
    bit_shrink.h
    bit_shrink.cpp)
find_package(Eigen3 REQUIRED)

# target_compile_options(Schem BEFORE PUBLIC -std=c++17)
target_link_libraries(Schem PUBLIC NBTWriter Eigen3::Eigen)
target_compile_features(Schem PRIVATE cxx_std_20)
target_link_libraries(Schem PUBLIC MCDataVersion ProcessBlockId)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(Schem PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()