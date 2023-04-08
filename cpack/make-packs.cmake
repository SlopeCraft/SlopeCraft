# Common attributes
set(CPACK_PACKAGE_NAME SlopeCraft)
set(CPACK_PACKAGE_VERSION ${SlopeCraft_version})
set(CPACK_PACKAGE_VENDOR "SlopeCraft group")
set(CPACK_PACKAGE_CONTACT https://github.com/SlopeCraft)
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.com/SlopeCraft)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-debug)
endif()



include(${CMAKE_SOURCE_DIR}/cpack/deb.cmake)

include(CPack)