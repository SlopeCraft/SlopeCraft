cmake_minimum_required(VERSION 3.18)
include_guard(GLOBAL)

set(CMAKE_CROSSCOMPILING ON)
set(CMAKE_SYSTEM_NAME "Darwin")
set(CMAKE_SYSTEM_PROCESSOR "arm64")
set(CMAKE_C_COMPILER_TARGET "arm64-apple-darwin")
set(CMAKE_CXX_COMPILER_TARGET "arm64-apple-darwin")
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET 12)

set(TARGET_SYSROOT "")
set(CMAKE_SYSROOT ${TARGET_SYSROOT})

#set(ENV{PKG_CONFIG_PATH} $PKG_CONFIG_PATH:/usr/lib/aarch64-linux-gnu/pkgconfig)
#set(ENV{PKG_CONFIG_LIBDIR} /usr/lib/pkgconfig:/usr/share/pkgconfig/:${TARGET_SYSROOT}/usr/lib/aarch64-linux-gnu/pkgconfig:${TARGET_SYSROOT}/usr/lib/pkgconfig)
#set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})

# if you use other version of gcc and g++ than gcc/g++ 9, you must change the following variables
set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${TARGET_SYSROOT}/usr/include")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

set(QT_COMPILER_FLAGS "-march=arm64")
set(QT_COMPILER_FLAGS_RELEASE "-O3 -pipe")
#set(QT_LINKER_FLAGS "-Wl,-O -Wl,--hash-style=gnu -Wl,--as-needed")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH ${TARGET_SYSROOT})


include(CMakeInitializeConfigs)

function(cmake_initialize_per_config_variable _PREFIX _DOCSTRING)
    if (_PREFIX MATCHES "CMAKE_(C|CXX|ASM)_FLAGS")
        set(CMAKE_${CMAKE_MATCH_1}_FLAGS_INIT "${QT_COMPILER_FLAGS}")

        foreach (config DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
            if (DEFINED QT_COMPILER_FLAGS_${config})
                set(CMAKE_${CMAKE_MATCH_1}_FLAGS_${config}_INIT "${QT_COMPILER_FLAGS_${config}}")
            endif ()
        endforeach ()
    endif ()


    if (_PREFIX MATCHES "CMAKE_(SHARED|MODULE|EXE)_LINKER_FLAGS")
        foreach (config SHARED MODULE EXE)
            set(CMAKE_${config}_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS}")
        endforeach ()
    endif ()

    _cmake_initialize_per_config_variable(${ARGV})
endfunction()

#set(XCB_PATH_VARIABLE ${TARGET_SYSROOT})
#
#set(GL_INC_DIR ${TARGET_SYSROOT}/usr/include)
#set(GL_LIB_DIR ${TARGET_SYSROOT}:${TARGET_SYSROOT}/usr/lib/aarch64-linux-gnu/:${TARGET_SYSROOT}/usr:${TARGET_SYSROOT}/usr/lib)
#
#set(EGL_INCLUDE_DIR ${GL_INC_DIR})
#set(EGL_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libEGL.so)
#
#set(OPENGL_INCLUDE_DIR ${GL_INC_DIR})
#set(OPENGL_opengl_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libOpenGL.so)
#
#set(GLESv2_INCLUDE_DIR ${GL_INC_DIR})
#set(GLESv2_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libGLESv2.so)
#
#set(gbm_INCLUDE_DIR ${GL_INC_DIR})
#set(gbm_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libgbm.so)
#
#set(Libdrm_INCLUDE_DIR ${GL_INC_DIR})
#set(Libdrm_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libdrm.so)
#
#set(XCB_XCB_INCLUDE_DIR ${GL_INC_DIR})
#set(XCB_XCB_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/aarch64-linux-gnu/libxcb.so)