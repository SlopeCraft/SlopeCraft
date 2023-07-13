

function(SC_download url filename)
    if (EXISTS ${filename})
        file(SIZE ${filename} size)
        if (${size} GREATER 0)
            return()
        endif ()

        file(REMOVE ${filename})
    endif ()

    message(STATUS "Downloading ${url} ...")
    file(DOWNLOAD ${url} ${filename}
        SHOW_PROGRESS
        REQUIRED)

    file(SIZE ${filename} size)
    if (${size} LESS_EQUAL 0)
        message(FATAL_ERROR "Failed to download ${filename} from ${url}, downloaded file is empty")
    endif ()
endfunction(SC_download)

set(DLLD_download_dir ${CMAKE_SOURCE_DIR}/3rdParty/DLLDeployer)
set(DLLD_file ${DLLD_download_dir}/DLLDeployer.cmake)
set(QD_file ${DLLD_download_dir}/QtDeployer.cmake)

SC_download(https://github.com/SlopeCraft/DLLDeployer/releases/download/v1.3/DLLDeployer.cmake
    ${DLLD_file})
include(${DLLD_file})

SC_download(https://github.com/SlopeCraft/DLLDeployer/releases/download/v1.3/QtDeployer.cmake
    ${QD_file})
include(${QD_file})