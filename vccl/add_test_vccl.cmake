
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_vccl_images)

set(temp_testname_prefix test_vccl_images/)

file(GLOB test_source_images "${CMAKE_SOURCE_DIR}/binaries/images/*")

set(test_source_images_space_list)

foreach (img ${test_source_images})
    if (${img} MATCHES " ")
        set(test_source_images_space_list "${test_source_images_space_list} \"${img}\"")
    else ()
        set(test_source_images_space_list "${test_source_images_space_list} ${img}")
    endif ()
endforeach (img ${test_source_images})
string(LENGTH ${test_source_images_space_list} len)
math(EXPR len "${len}-1")
string(SUBSTRING ${test_source_images_space_list} 1 ${len} test_source_images_space_list)
unset(len)


message(STATUS "test_source_images_space_list = ${test_source_images_space_list}")

set(list_faces "up" "down" "north" "south" "east" "west")

#set(algos "RGB" "RGB+" "HSV" "Lab94" "Lab00")
set(algos "RGB")

set(dither "false")

cmake_policy(PUSH)

cmake_policy(SET CMP0110 OLD)


if (${SlopeCraft_GPU_API} STREQUAL "None")
    set(gpu_flags)
else ()
    set(gpu_flags --gpu --platform ${SlopeCraft_vccl_test_gpu_platform_idx} --device ${SlopeCraft_vccl_test_gpu_device_idx})
endif ()

# set(dither "true" "false")
foreach (_layers RANGE 1 3 1)
    foreach (_ver RANGE 12 21)
        set(VCL_current_var_name VCL_resource_${mcver})
        set(zip_file ${VCL_current_var_name})

        set(generate_schem)

        if (NOT _ver EQUAL 12)
            set(generate_schem "--schem")
        endif ()

        foreach (_face ${list_faces})
            foreach (_algo ${algos})
                foreach (_dither ${dither})
                    # message(STATUS ${temp_testname_prefix}_${_ver}_${_face})
                    set(test_name ${temp_testname_prefix}ver=${_ver}_face=${_face}_layer=${_layers}_algo=${_algo}_dither=${_dither}_)

                    # message(STATUS ${test_name})
                    add_test(NAME ${test_name}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

                        # COMMAND vccl --bsl ${CMAKE_SOURCE_DIR}/VisualCraftL/VCL_blocks_fixed.json --rp ${zip_file} --mcver ${_ver} --face ${_face} --layers ${_layers} --img ${VCL_test_images} --dither=${_dither} -j1 --out-image --benchmark --prefix ${test_name} --gpu --disable-config
                        COMMAND vccl --img ${test_source_images} --mcver ${_ver} --face ${_face} --layers ${_layers} --dither=${_dither} -j20 --out-image --benchmark --prefix ${test_name} --lite --nbt ${generate_schem} ${gpu_flags}
                        COMMAND_EXPAND_LISTS
                    )

                    #
                endforeach (_dither ${dither})
            endforeach (_algo ${algos})
        endforeach (_face ${list_faces})
    endforeach (_ver RANGE 12 21)
endforeach (_layers RANGE 1 3 1)


cmake_policy(POP)