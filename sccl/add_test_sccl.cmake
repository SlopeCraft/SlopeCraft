
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_sccl)

file(GLOB test_source_images ${CMAKE_SOURCE_DIR}/binaries/images/*)

message(STATUS "Test images for sccl: ${test_source_images}")

set(list_algos "RGB" "RGB_Better" "Lab94" "Lab00" "HSV" "XYZ")
set(list_type "Slope" "Flat" "FileOnly")
#set(list_)

#message(STATUS "version list: ${VCL_version_names}")

set(index 0)
foreach (_ver ${VCL_version_names})
    set(version_name "MC${_ver}")
    foreach (algo ${list_algos})
        foreach (type ${list_type})

            set(test_name "sccl-${version_name}-${algo}-${type}")

            set(dither_option)
            math(EXPR dither "${index} % 2")
            if (${dither} EQUAL 1)
                set(dither_option "--dither")
                set(test_name "${test_name}-dither")
            endif ()

            set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/test_sccl/${test_name}")
            file(MAKE_DIRECTORY ${output_dir})


            set(build_options)
            if (${type} STREQUAL "Slope")
                list(APPEND build_options --lossless-compression --lossy-compression --max-height=50 --glass-bridge=withBridge --glass-bridge-interval=5 --fire-proof --connect-mushroom)
            endif ()

            set(export_options)
            if (${type} STREQUAL "Flat")
                list(APPEND export_options "export-flat-diagram")
            endif ()
            if ((${type} STREQUAL "Slope") OR (${type} STREQUAL "Flat"))
                set(export_options export-litematica export-structure)
                if (NOT ${version_name} STREQUAL "MC12")
                    list(APPEND export_options "export-WEschem")
                endif ()
            endif ()

            add_test(NAME ${test_name}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND sccl --build-dir-mode --image=${test_source_images} --mcver=${version_name} --map-type=${type} --algo=${algo} ${build_options} ${dither_option} --export-prefix=${output_dir} export-converted-image export-map-data-file --begin-index=100 ${export_options}
                COMMAND_EXPAND_LISTS
            )

            math(EXPR index "${index} + 1")
        endforeach ()
    endforeach ()
endforeach ()