
include(CheckCXXCompilerFlag)
#add_definitions("-DSC_VECTORIZE_AVX2")

message(STATUS "System processor arch = ${CMAKE_SYSTEM_PROCESSOR}")

set(amd64_arch_names AMD64 x86_64)

if (${CMAKE_SYSTEM_PROCESSOR} IN_LIST amd64_arch_names)
    if (${MSVC})
        set(SlopeCraft_vectorize_flags "/arch:AVX2")
    else ()
        set(SlopeCraft_vectorize_flags -mavx -mavx2 -mfma)
    endif ()
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
    set(SlopeCraft_vectorize_flags -mfpu=neon)
    #    if (${APPLE})
    #        set(SlopeCraft_vectorize_flags -mcpu=apple-m1 -mfpu=neon)
    #    endif ()
else ()
    message(WARNING "Unknown cpu arch \"${CMAKE_SYSTEM_PROCESSOR}\", using -march=native")
endif ()


message(STATUS "Vectorize using " ${SlopeCraft_vectorize_flags})
add_compile_options(${SlopeCraft_vectorize_flags})