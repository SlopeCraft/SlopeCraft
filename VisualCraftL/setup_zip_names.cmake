set(VCL_app_files
    ${CMAKE_SOURCE_DIR}/VisualCraftL/VCL_blocks_fixed.json)

foreach (mcver RANGE 12 21)
    set(VCL_current_var_name VCL_resource_${mcver})

    if (NOT DEFINED ${VCL_current_var_name})
        message(WARNING "${VCL_current_var_name} is not defined")
    endif ()

    list(APPEND VCL_app_files ${${VCL_current_var_name}})
endforeach (mcver RANGE 12 21)

unset(mcver)