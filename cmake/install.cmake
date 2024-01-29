set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

include(${CMAKE_SOURCE_DIR}/cmake/add_compiler_path_to_prefix.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/SlopeCraftConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_BINARY_DIR}/SlopeCraftConfigVersion.cmake
    DESTINATION lib/cmake/SlopeCraft)

install(EXPORT SlopeCraftTargets
    FILE SlopeCraftTargets.cmake
    NAMESPACE SlopeCraft::
    DESTINATION lib/cmake/SlopeCraft)

if (${WIN32})
    #include(${CMAKE_SOURCE_DIR}/cmake/scan_deps_for_lib.cmake)
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION .)

    #    set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
    #
    #    function(SlopeCraft_install_dll dllname dest is_required)
    #        find_library(dll_file
    #            NAMES ${dllname}
    #            PATHS ${CMAKE_PREFIX_PATH}
    #            PATH_SUFFIXES bin
    #            NO_CMAKE_INSTALL_PREFIX
    #            NO_CACHE)
    #
    #        if (NOT dll_file)
    #            find_file(dll_file
    #                NAMES "lib${dllname}.dll"
    #                PATHS ${CMAKE_PREFIX_PATH}
    #                PATH_SUFFIXES bin
    #                NO_CMAKE_INSTALL_PREFIX
    #                NO_CACHE)
    #        endif ()
    #
    #        if (NOT dll_file)
    #            if (${is_required})
    #                message(FATAL_ERROR "Failed to find ${dllname} dll.")
    #            else ()
    #                message(WARNING "Failed to find ${dllname} dll.")
    #            endif ()
    #            return()
    #        endif ()
    #
    #        message(STATUS "Install ${dll_file} to ${dest}")
    #        install(FILES ${dll_file}
    #            DESTINATION ${dest})
    #    endfunction(SlopeCraft_install_dll)
    #
    #    if (${MSVC})
    #        SlopeCraft_install_dll(omp . true)
    #    else ()
    #        SlopeCraft_install_dll(gomp-1 . true)
    #        SlopeCraft_install_dll(stdc++-6 . true)
    #        SlopeCraft_install_dll(gcc_s_seh-1 . true)
    #        SlopeCraft_install_dll(winpthread-1 . true)
    #    endif ()

    return()
endif ()

if (${LINUX})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION .)
    return()
endif ()

if (${APPLE})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION .)
    return()
endif ()

message(WARNING "No rule to install readme and license files. Unknown system name.")