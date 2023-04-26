set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SlopeCraft group")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)

set(SlopeCraft_debian_opencl_deps)

if(${SlopeCraft_GPU_API} STREQUAL "OpenCL")
    set(SlopeCraft_debian_opencl_deps
        ocl-icd-libopencl1
    )
endif()

set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libzip4"
    "libpng16-16"
    "libqt6core6(>=6.2.4)"
    "libqt6gui6(>=6.2.4)"
    "libqt6widgets6(>=6.2.4)"
    "libqt6network6(>=6.2.4)"
    "libopengl0"
    "libqt6opengl6(>=6.2.4)"
    "libqt6openglwidgets6(>=6.2.4)"
    "libqt6dbus6(>=6.2.4)"
    "libglx0"
    "qt6-qpa-plugins(>=6.2.4)"
    ${SlopeCraft_debian_opencl_deps}
)

list(JOIN CPACK_DEBIAN_PACKAGE_DEPENDS "," CPACK_DEBIAN_PACKAGE_DEPENDS)

message(STATUS "CPACK_DEBIAN_PACKAGE_DEPENDS = ${CPACK_DEBIAN_PACKAGE_DEPENDS}")

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "Get your Minecraft pixel painting in multiple kinds of forms")