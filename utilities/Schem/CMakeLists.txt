project(Schem VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
add_library(Schem

    Schem.h
    Schem.cpp
    bit_shrink.h
    bit_shrink.cpp
    mushroom.h
    mushroom.cpp
    entity.h
    entity.cpp
    item.cpp
    item.h
)
find_package(Eigen3 REQUIRED)

find_package(cereal REQUIRED)
find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)

find_package(Boost)

# target_compile_options(Schem BEFORE PUBLIC -std=c++17)
target_link_libraries(Schem PUBLIC
    NBTWriter
    Eigen3::Eigen
    cereal::cereal
    fmt::fmt
    magic_enum::magic_enum
    NBTWriter
)
if (TARGET Boost::multi_array)
    target_link_libraries(Schem PUBLIC Boost::multi_array)
endif ()
target_compile_features(Schem PUBLIC cxx_std_23)
target_link_libraries(Schem PUBLIC MCDataVersion ProcessBlockId)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(Schem PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif ()