cmake_minimum_required(VERSION 3.16)

# Common attributes
set(CPACK_PACKAGE_NAME SlopeCraft)
set(CPACK_PACKAGE_VERSION ${SlopeCraft_version})
set(CPACK_PACKAGE_VENDOR "SlopeCraft group")
set(CPACK_PACKAGE_CONTACT https://github.com/SlopeCraft)
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.com/SlopeCraft)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-debug)
endif ()

if (${APPLE})
    set(cpu_arch ${CMAKE_SYSTEM_PROCESSOR})
    if (${cpu_arch} STREQUAL "AMD64")
        set(cpu_arch "x86_64")
    endif ()

    if (${cpu_arch} STREQUAL "x86_64")
        set(CPACK_SYSTEM_NAME "macOS-Intel")
    endif ()
    if (${cpu_arch} STREQUAL "arm64")
        set(CPACK_SYSTEM_NAME "macOS-Silicon")
    endif ()
endif ()

include(${CMAKE_SOURCE_DIR}/cpack/deb.cmake)

include(CPack)