if (NOT APPLE)
    return()
endif ()

set(DylibD_download_dir ${CMAKE_SOURCE_DIR}/3rdParty/DylibDeployer.cmake)
set(DylibD_file ${DylibD_download_dir}/DylibDeployer.cmake)
file(DOWNLOAD https://github.com/SlopeCraft/DylibDeployer.cmake/releases/download/v1.0.2/DylibDeployer.cmake
    ${DylibD_file}
    EXPECTED_HASH SHA512=22D8E28508C424EA724294B58A22559747E33D0529358582E5497D613074204D3584FACB4FBBC85ACC3DBDA5CA67F666C7BFEE2BBF2DC02129871E354B5BC25B
)
include(${DylibD_file})

set(Codesigner_file ${DylibD_download_dir}/Codesigner.cmake)
file(DOWNLOAD https://github.com/SlopeCraft/DylibDeployer.cmake/releases/download/v1.0.2/Codesigner.cmake
    ${Codesigner_file}
    EXPECTED_HASH SHA512=2095462FB94B5F7D25953EED7ECDBA89A412B31E8C48B31CA328445106498EC088C08F35CA0C4498D6D22C60D8A26605D506DAA29E1A3D276316C5FD7FFE015E
)
include(${Codesigner_file})