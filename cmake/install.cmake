set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

function(SlopeCraft_install_dll dllname dest is_required)
    find_library(dll_file
        NAMES ${dllname}
        PATHS ${CMAKE_PREFIX_PATH}
        NO_CMAKE_INSTALL_PREFIX
        NO_CACHE)

    if(NOT dll_file)
        if(${is_required})
            message(WARNING "Failed to find ${dllname} dll.")
        else()
            message(STATUS "Failed to find ${dllname} dll.")
        endif()
        return()
    endif()

    message(STATUS "Install ${dll_file} to ${dest}")
    install(FILES ${dll_file}
        DESTINATION ${dest})
endfunction(SlopeCraft_install_dll)

if(${WIN32})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX})

    set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")

    if(${MSVC})
    else()
        SlopeCraft_install_dll(gomp-1 . true)
        SlopeCraft_install_dll(stdc++-6 . true)
        SlopeCraft_install_dll(gcc_s_seh-1 . true)
        SlopeCraft_install_dll(winpthread-1 . true)
    endif()

    return()
endif()

if(${LINUX})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION .)
    return()
endif()

if(${APPLE})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX})
    return()
endif()

message(WARNING "No rule to install readme and license files. Unknown system name.")