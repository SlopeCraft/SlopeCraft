set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


add_library(StatMemory STATIC stat_memory.h)
find_package(tl-expected REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(StatMemory PUBLIC tl::expected fmt::fmt)
target_include_directories(StatMemory PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_features(StatMemory PUBLIC cxx_std_23)
# Set source file by system
if (${WIN32})
    target_sources(StatMemory PRIVATE stat_memory_src/windows.cpp)
    target_link_libraries(StatMemory PUBLIC psapi.lib)
elseif (${LINUX})
    target_sources(StatMemory PRIVATE stat_memory_src/linux.cpp)
elseif (${APPLE})
    target_sources(StatMemory PRIVATE stat_memory_src/macos.cpp)
elseif ()
    message(FATAL_ERROR "Unsupported system ${CMAKE_SYSTEM_NAME}, not windows, linux or macos")
endif ()

add_executable(test_stat_memory others/test_stat_memory.cpp)
target_link_libraries(test_stat_memory PRIVATE StatMemory)
add_test(NAME stat_memory
    COMMAND test_stat_memory)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
add_library(MemoryPolicyDialog STATIC
    MemoryPolicyDialog.h
    MemoryPolicyDialog.cpp
    MemoryPolicyDialog.ui
)
target_link_libraries(MemoryPolicyDialog PUBLIC Qt6::Widgets)
target_link_libraries(MemoryPolicyDialog PUBLIC StatMemory)

qt_add_lupdate(MemoryPolicyDialog
    TS_FILES others/MemoryPolicyDialog_en_US.ts
    OPTIONS ${SC_lupdate_flags}
)
qt_add_lrelease(MemoryPolicyDialog
    TS_FILES others/MemoryPolicyDialog_en_US.ts
    QM_FILES_OUTPUT_VARIABLE MemoryPolicyDialog_qm_files
)
qt_add_resources(MemoryPolicyDialog "MPD_translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${MemoryPolicyDialog_qm_files}
)