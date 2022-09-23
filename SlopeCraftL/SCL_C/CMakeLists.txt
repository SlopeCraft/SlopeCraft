cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_SlopeCraftL VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ../../GAConverter
)


add_definitions(-DSLOPECRAFTL_LIBRARY)
add_definitions(-DSCL_CAPI)

add_library(SlopeCraftL_C SHARED ${SlopeCraft_SCL_sources})
# link SlopeCraftL with GAConverter and zlib
target_link_libraries(SlopeCraftL_C PUBLIC GAConverter -lz)

set_target_properties(SlopeCraftL_C PROPERTIES
    PREFIX ""
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)


install(TARGETS SlopeCraftL_C
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/C/lib 
    #LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/C/lib
    )



set(SlopeCraft_SCL_C_include_files 
    
    ../SlopeCraftL.h 
    SlopeCraftL_global.h # a copy from ../SlopeCraft_global.h, an extra line `#define SCL_CAPI` is added in this file 
    )

install(FILES ${SlopeCraft_SCL_C_include_files}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/C/include)