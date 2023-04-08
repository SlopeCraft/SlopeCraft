set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

if(${WIN32})
    install(FILES ${SlopeCraft_readme_and_license_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX})
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