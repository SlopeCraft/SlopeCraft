find_package(Qt6 COMPONENTS Core REQUIRED)

if (${WIN32})
    set(source_file windows.cpp)
elseif (${APPLE})
    set(source_file macOS.cpp)
else ()
    set(source_file linux.cpp)
endif ()

add_library(cpuInfo STATIC
    cpu_info.h
    ${source_file}
)
target_include_directories(cpuInfo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpuInfo PUBLIC Qt6::Core)
target_compile_features(cpuInfo PRIVATE cxx_std_23)
set_target_properties(cpuInfo PROPERTIES POSITION_INDEPENDENT_CODE ON)