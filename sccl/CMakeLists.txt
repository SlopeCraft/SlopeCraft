project(sccl VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(sccl
    main.cpp
    sccl_internal.h
    run.cpp
)

find_package(Qt6 COMPONENTS LinguistTools REQUIRED)
find_package(magic_enum REQUIRED)
find_package(OpenMP REQUIRED)

target_compile_features(sccl PRIVATE cxx_std_23)
target_link_libraries(sccl PRIVATE
    SlopeCraftL
    Qt6::Core
    OpenMP::OpenMP_CXX
    magic_enum::magic_enum
    MCDataVersion
    BlockListManager
)
target_include_directories(sccl PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
)
set_target_properties(sccl PROPERTIES
    VERSION ${PROJECT_VERSION}

    # MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

if (${WIN32})
    configure_file(others/sccl.rc.in ${CMAKE_CURRENT_BINARY_DIR}/sccl.rc)
    target_sources(sccl PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/sccl.rc
    )

    add_custom_target(SC_create_symlink_SCCL
        COMMAND mklink SlopeCraftL.dll "..\\SlopeCraftL\\SlopeCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS SlopeCraftL
        COMMENT "Create symlink to SlopeCraftL.dll for sccl.exe"
    )
    add_dependencies(SC_create_all_symlinks SC_create_symlink_SCCL)
endif ()