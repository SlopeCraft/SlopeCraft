set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
add_library(MemoryPolicyDialog STATIC
    MemoryPolicyDialog.h
    MemoryPolicyDialog.cpp
    MemoryPolicyDialog.ui
)
target_link_libraries(MemoryPolicyDialog PUBLIC Qt6::Widgets)


add_library(StatMemory STATIC stat_memory.h)
find_package(tl-expected REQUIRED)
target_link_libraries(StatMemory PUBLIC tl::expected)
target_include_directories(StatMemory PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(MemoryPolicyDialog PUBLIC StatMemory)

if (${WIN32})
    target_sources(StatMemory PRIVATE stat_memory_windows.cpp)
    target_link_libraries(StatMemory PUBLIC psapi.lib)
    return()
endif ()

message(FATAL_ERROR "Unsupported system ${CMAKE_SYSTEM_NAME}, not windows, linux or macos")