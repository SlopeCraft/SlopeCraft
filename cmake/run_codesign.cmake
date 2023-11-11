if (DEFINED CMAKE_GENERATOR)
    message(STATUS "Running at configuration time")
    option(RCS_configure_time "Whether this script is running at configuration time" ON)
else ()
    # Otherwise we guess that it's running at build or installation time.
    message(STATUS "Running at build/install time")
    option(RCS_configure_time "Whether this script is running at configuration time" OFF)
endif ()

option(RCS_run_directly "" OFF)
if(RCS_configure_time)
    set(RCS_source_file ${CMAKE_CURRENT_LIST_FILE})
else()
    set(RCS_this_file @rcs_this_file@)
    set(RCS_bundle_name @rcs_bundle_name@)
    set(RCS_bundle_version @rcs_bundle_version@)

    set(RCS_working_dir . CACHE FILEPATH "")
endif()


function(RCS_add_codesign target)
    set(rcs_this_file "${CMAKE_CURRENT_BINARY_DIR}/run_codesign_install.cmake")
    set(rcs_bundle_name ${target})
    get_target_property(rcs_bundle_version ${target} VERSION)

    configure_file(${RCS_source_file}
        ${rcs_this_file}
        @ONLY)

    install(SCRIPT ${rcs_this_file}
        DESTINATION .)
endfunction()

if(NOT RCS_configure_time)
    if(NOT RCS_run_directly)

        message(STATUS "RCS_working_dir = \"${RCS_working_dir}\"")

        execute_process(COMMAND ${CMAKE_COMMAND} -DRCS_run_directly:BOOL=ON -DRCS_working_dir=${CMAKE_INSTALL_PREFIX} -P ${RCS_this_file}
            WORKING_DIRECTORY ${RCS_working_dir}
            COMMAND_ERROR_IS_FATAL ANY)
        return()
    endif()

    set(bundle_prefix "${RCS_working_dir}/${RCS_bundle_name}.app/Contents/MacOS")

    if(IS_SYMLINK "${bundle_prefix}/${RCS_bundle_name}")
        message(STATUS "Found symlink file ${bundle_prefix}/${RCS_bundle_name}")

        file(REMOVE "${bundle_prefix}/${RCS_bundle_name}")
        file(RENAME "${bundle_prefix}/${RCS_bundle_name}-${RCS_bundle_version}" "${bundle_prefix}/${RCS_bundle_name}")
    endif ()

    if(IS_SYMLINK "${bundle_prefix}/${RCS_bundle_name}")
        message(WARNING "\"${bundle_prefix}/${RCS_bundle_name}\" is a symlink, but it should be regular file.")
    endif ()

    execute_process(COMMAND codesign --force --deep --sign=- ${RCS_bundle_name}.app
        WORKING_DIRECTORY ${RCS_working_dir}
        COMMAND_ERROR_IS_FATAL ANY)
endif()