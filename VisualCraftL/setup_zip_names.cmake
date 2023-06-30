set(VCL_app_files
        ${CMAKE_SOURCE_DIR}/VisualCraftL/VCL_blocks_fixed.json)

include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips.cmake)

foreach (mcver RANGE 12 20)
    set(VCL_current_var_name VCL_resource_${mcver})

    if (NOT DEFINED ${VCL_current_var_name})
        message(WARNING "${VCL_current_var_name} is not defined")
    endif ()

    list(APPEND VCL_app_files ${${VCL_current_var_name}})
endforeach (mcver RANGE 12 20)

unset(mcver)