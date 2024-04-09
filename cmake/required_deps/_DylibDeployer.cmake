if (NOT APPLE)
    return()
endif ()

include(_DLLDeployer.cmake)

set(DylibD_download_dir ${CMAKE_SOURCE_DIR}/3rdParty/DylibDeployer.cmake)
set(DylibD_file ${DylibD_download_dir}/DylibDeployer.cmake)
SC_download(https://github.com/SlopeCraft/DylibDeployer.cmake/releases/download/v1.0.2/DylibDeployer.cmake
    ${DylibD_file})
include(${DylibD_file})

set(Codesigner_file ${DylibD_download_dir}/Codesigner.cmake)
SC_download(https://github.com/SlopeCraft/DylibDeployer.cmake/releases/download/v1.0.2/Codesigner.cmake
    ${Codesigner_file})
include(${Codesigner_file})