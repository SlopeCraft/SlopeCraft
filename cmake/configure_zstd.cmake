find_package(ZSTD
    CONFIG
    REQUIRED)

set(SC_zstd_target_name "")
if (TARGET zstd::libzstd_shared)
    set(SC_zstd_target_name "zstd::libzstd_shared")
elseif (TARGET zstd::zstd)
    set(SC_zstd_target_name "zstd::zstd")
elseif (TARGET zstd::libzstd_static)
    set(SC_zstd_target_name "zstd::libzstd_static")
elseif ()
    message(FATAL_ERROR "No zstd library target imported.")
endif ()
message(STATUS "Found zstd: ${SC_zstd_target_name}")