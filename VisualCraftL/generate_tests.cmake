include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)

set(temp_testname_prefix test_block_list)

set(list_faces "up" "down" "north" "south" "east" "west")

foreach (_layers RANGE 1 3 1)
    foreach (_ver RANGE 12 20)
        if (_ver EQUAL 12)
            set(zip_file ${VCL_resource_12})
        else ()
            set(zip_file ${VCL_resource_latest})
        endif ()

        foreach (_face ${list_faces})
            # message(STATUS ${temp_testname_prefix}_${_ver}_${_face})
            set(test_name ${temp_testname_prefix}_${_ver}_${_face}_layer${_layers})

            # message(STATUS ${test_name})
            add_test(NAME ${test_name}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMAND test_VCL_blockstate ${CMAKE_CURRENT_SOURCE_DIR}/VCL_blocks_fixed.json ${zip_file} --version ${_ver} --face ${_face} --layers ${_layers})
        endforeach (_face ${list_faces})
    endforeach (_ver RANGE 12 20)
endforeach (_layers RANGE 1 3 1)

# add_test(NAME test_block_list_01
# WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# COMMAND test_VCL_blockstate ${CMAKE_CURRENT_SOURCE_DIR}/VCL_blocks_fixed.json Vanilla_1_19_2.zip)