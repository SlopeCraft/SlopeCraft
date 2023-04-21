cmake_minimum_required(VERSION 3.16)

# Common attributes
set(CPACK_PACKAGE_NAME SlopeCraft)
set(CPACK_PACKAGE_VERSION ${SlopeCraft_version})
set(CPACK_PACKAGE_VENDOR "SlopeCraft group")
set(CPACK_PACKAGE_CONTACT https://github.com/SlopeCraft)
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.com/SlopeCraft)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-debug)
endif()

# set(CPACK_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-${SlopeCraft_GPU_API})
if(${WIN32})
    configure_file(${CMAKE_SOURCE_DIR}/cpack/pack_deployed_files.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/pack_deployed_files.cmake
        @ONLY)
    list(APPEND CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/pack_deployed_files.cmake)

    # set(CPACK_PACKAGE_FILES)
endif()

include(${CMAKE_SOURCE_DIR}/cpack/deb.cmake)

include(CPack)