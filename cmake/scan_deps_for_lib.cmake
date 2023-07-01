if (NOT ${WIN32})
    message(FATAL_ERROR "This cmake script is included with incorrect system, expected windows.")
endif ()


find_file(SC_MSVC_lib_exe lib.exe)


function(scan_deps_of_msvc_lib_file lib_file_name dest_depend_files)

    if (NOT SC_MSVC_lib_exe)
        message(FATAL_ERROR "You are using msvc-like compiler on windows, but cmake failed to find lib.exe of visual studio.")
    endif ()


    execute_process(COMMAND ${SC_MSVC_lib_exe} /list ${lib_file_name}
            OUTPUT_VARIABLE MSVC_lib_output COMMAND_ERROR_IS_FATAL ANY)

    #message(STATUS "output of ${lib_file_name} is : ${MSVC_lib_output}")
    string(REPLACE "\n" ";" MSVC_lib_output ${MSVC_lib_output})
    set(temp)
    foreach (item ${MSVC_lib_output})
        if ((${item} MATCHES "Microsoft") OR (${item} MATCHES "Copyright"))
            message(STATUS "Filter out ${item}")
            continue()
        endif ()
        list(APPEND temp ${item})
    endforeach ()

    set(${dest_depend_files} ${temp} PARENT_SCOPE)
endfunction()

function(is_export_library lib_file_name dest_is_export_lib)


    if ((${lib_file_name} MATCHES ".dll.a"))
        message(STATUS "${lib_file_name} is a export lib.")
        set({dest_is_export_lib} ON PARENT_SCOPE)
        return()
    endif ()

    cmake_path(GET lib_file_name EXTENSION extension)

    if ((${extension} STREQUAL ".a") OR (${extension} STREQUAL ".obj") OR (${extension} STREQUAL ".o") OR (${extension} STREQUAL ".dll"))
        message(STATUS "${lib_file_name} is a static or dynamic lib or a object.")
        set({dest_is_export_lib} OFF PARENT_SCOPE)
        return()
    endif ()

    if (NOT ${extension} STREQUAL ".lib")
        message(FATAL_ERROR "Failed to deduce library type of ${lib_file_name}")
    endif ()

    scan_deps_of_msvc_lib_file(${lib_file_name} dep_list)
    message(STATUS "dep_list = ${dep_list}")

    foreach (dep_file ${dep_list})
        message("dep_file = ${dep_file}")
        if (${dep_file} MATCHES ".dll")
            message(STATUS "${lib_file_name} is a export lib.")
            set(${dest_is_export_lib} ON PARENT_SCOPE)
            return()
        endif ()
    endforeach ()
    message(STATUS "${lib_file_name} is a static lib.")
    set(${dest_is_export_lib} OFF PARENT_SCOPE)


endfunction()