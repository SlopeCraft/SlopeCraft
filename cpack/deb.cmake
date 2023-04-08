set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SlopeCraft group")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE x86_64)

set(SlopeCraft_debian_opencl_deps)
if(${SlopeCraft_GPU_API})
    set(SlopeCraft_debian_opencl_deps 
        ocl-icd-libopencl1
        )
endif()

set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    libzip4 
    libpng16-16
    libqt6core6
    libqt6gui6
    libqt6widgets6
    libqt6network6
    libqt6concurrent6
    libopengl0
    libqt6opengl6
    libqt6openglwidgets6
    libqt6dbus6
    libglx0
    qt6-qpa-plugins
    ${SlopeCraft_debian_opencl_deps}
    )

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION 
    "Get your Minecraft pixel painting in multiple kinds of forms")