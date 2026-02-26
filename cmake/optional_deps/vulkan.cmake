find_package(Vulkan
    OPTIONAL_COMPONENTS glslangValidator
    REQUIRED
)

if (NOT TARGET Vulkan::glslangValidator)
    message(STATUS "cyka, glslangValidator is not found by find_package. Searching is manually...")
    # Strategy for vcpkg. In vcpkg, executables are put at tools/...
    set(path_hints)
    foreach (prefix ${CMAKE_PREFIX_PATH})
        list(APPEND path_hints "${prefix}/tools/glslang")
    endforeach ()
    find_program(glslangValidator_loc
        NAMES glslangValidator
        HINTS ${path_hints}
        REQUIRED
    )
    message(STATUS "Manually found glslangValidator at ${glslangValidator_loc}")
    add_executable(glslangValidator_imported IMPORTED)
    set_target_properties(glslangValidator_imported PROPERTIES
        IMPORTED_LOCATION ${glslangValidator_loc}
    )
    add_executable(Vulkan::glslangValidator ALIAS glslangValidator_imported)
endif ()

get_target_property(glslangValidator_exe Vulkan::glslangValidator LOCATION)
message(STATUS "glslangValidator found at ${glslangValidator_exe}")
