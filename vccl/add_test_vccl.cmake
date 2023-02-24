include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_images.cmake)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_vccl_images)

set(temp_testname_prefix test_vccl_images/)

set(list_faces "up" "down" "north" "south" "east" "west")

set(algos "RGB" "HSV" "Lab94" "XYZ")

set(dither "false")

# set(dither "true" "false")
foreach(_layers RANGE 1 3 1)
    foreach(_ver RANGE 12 19)
        if(_ver EQUAL 12)
            set(zip_file ${VCL_resource_12})
            set(generate_schem)
        else()
            set(zip_file ${VCL_resource_latest})
            set(generate_schem "--schem")
        endif()

        foreach(_face ${list_faces})
            foreach(_algo ${algos})
                foreach(_dither ${dither})
                    # message(STATUS ${temp_testname_prefix}_${_ver}_${_face})
                    set(test_name ${temp_testname_prefix}ver=${_ver}_face=${_face}_layer=${_layers}_algo=${_algo}_dither=${_dither}_)

                    # message(STATUS ${test_name})
                    add_test(NAME ${test_name}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

                        # COMMAND vccl --bsl ${CMAKE_SOURCE_DIR}/VisualCraftL/VCL_blocks_fixed.json --rp ${zip_file} --mcver ${_ver} --face ${_face} --layers ${_layers} --img ${VCL_test_images} --dither=${_dither} -j1 --out-image --benchmark --prefix ${test_name} --gpu --disable-config
                        COMMAND vccl --mcver ${_ver} --face ${_face} --layers ${_layers} --img ${CMAKE_SOURCE_DIR}/binaries/images/* --dither=${_dither} -j20 --out-image --benchmark --prefix ${test_name} --gpu --lite --nbt ${generate_schem}
                    )

                    #
                endforeach(_dither ${dither})
            endforeach(_algo ${algos})
        endforeach(_face ${list_faces})
    endforeach(_ver RANGE 12 19)
endforeach(_layers RANGE 1 3 1)