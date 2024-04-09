# https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/test_image_01.png

unset(VCL_test_images)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/images/test_image_01.png)
    set(VCL_test_images ${CMAKE_SOURCE_DIR}/binaries/images/test_image_01.png)
    message(STATUS "VCL testing images found.")
    return()
endif()

message(STATUS "Downloading VCL testing images...")

file(DOWNLOAD
    https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/test_image_01.png
    ${CMAKE_SOURCE_DIR}/binaries/images/test_image_01.png)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/images/test_image_01.png)
    set(VCL_test_images ${CMAKE_SOURCE_DIR}/binaries/images/test_image_01.png)
    message(STATUS "Downloaded testing images for VCL.")
else()
    message(FATAL_ERROR "Failed to testing images for VCL.")
endif()