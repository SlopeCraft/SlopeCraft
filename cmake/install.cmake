set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

if(${WIN32})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX})

    set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")

    find_library(libgomp_dll
        NAMES gomp-1
        PATHS ${CMAKE_PREFIX_PATH}
        NO_CMAKE_INSTALL_PREFIX)

    if(NOT libgomp_dll)
        message(WARNING "Failed to find libgomp dll.")
    else()
        install(FILES ${libgomp_dll}
            DESTINATION ${CMAKE_INSTALL_PREFIX})
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