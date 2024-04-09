#function(SC_download url filename)
#    if (EXISTS ${filename})
#        file(SIZE ${filename} size)
#        if (${size} GREATER 0)
#            return()
#        endif ()
#
#        file(REMOVE ${filename})
#    endif ()
#
#    message(STATUS "Downloading ${url} ...")
#    file(DOWNLOAD ${url} ${filename}
#        SHOW_PROGRESS
#        REQUIRED)
#
#    file(SIZE ${filename} size)
#    if (${size} LESS_EQUAL 0)
#        message(FATAL_ERROR "Failed to download ${filename} from ${url}, downloaded file is empty")
#    endif ()
#endfunction(SC_download)

set(DLLD_download_dir ${CMAKE_BINARY_DIR}/3rdParty/DLLDeployer)
set(DLLD_file ${DLLD_download_dir}/DLLDeployer.cmake)
set(QD_file ${DLLD_download_dir}/QtDeployer.cmake)
if (${WIN32})
    file(DOWNLOAD https://github.com/ToKiNoBug/SharedLibDeployer/releases/download/v1.4.0/DLLDeployer.cmake
        ${DLLD_file}
        EXPECTED_HASH SHA512=6044ACEA50BFC9EACA37357BF2CD1497908905DAE0971F74B4BE4BAED6633420BDD38105FE21E47724B9653DEC4880BEBCA1341DC95AA9BD8144356C7DAB1899)
    #    SC_download(https://github.com/ToKiNoBug/SharedLibDeployer/releases/download/v1.4.0/DLLDeployer.cmake
    #        ${DLLD_file})
    include(${DLLD_file})
endif ()

if (WIN32 OR APPLE)
    file(DOWNLOAD https://github.com/ToKiNoBug/SharedLibDeployer/releases/download/v1.4.0/QtDeployer.cmake
        ${QD_file}
        EXPECTED_HASH SHA512=7A6E7FEE90DEBC0A4E6BDC4CB65D87DF453E8B24C6ED9E188C174BF64C526CA232FA63211CA661E4B26D06A141291AB0AB3CC499B64775B23FC0E622E0CEA0B4)
    #    SC_download(https://github.com/SlopeCraft/DLLDeployer/releases/download/v1.3/QtDeployer.cmake
    #        ${QD_file})
    include(${QD_file})
endif ()
