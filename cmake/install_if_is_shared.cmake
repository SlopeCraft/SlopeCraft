function(SlopeCraft_install_lib lib_location dest)
    cmake_path(GET lib_location EXTENSION extension)

    # message(STATUS "extension of ${lib_location} is ${extension}")
    if((extension MATCHES ".dll.a") OR(extension MATCHES ".dll.lib"))
        cmake_path(GET lib_location STEM lib_stem)

        # message(STATUS "lib_stem = ${lib_stem}")
        set(dll_name "${lib_stem}.dll")

        # message(STATUS "dll_name = ${dll_name}")
        cmake_path(GET lib_location PARENT_PATH search_dir)
        cmake_path(GET search_dir PARENT_PATH search_dir)

        # message("search_dir = ${search_dir}")

        # find_file(dll_location
        # NAMES ${dll_name}
        # HINTS ${search_dir}/bin/${dll_name}
        # PATHS ${search_dir}
        # PATH_SUFFIXES bin
        # NO_CACHE)
        find_library(dll_location
            NAMES lib_stem
            PATHS ${search_dir}
            PATH_SUFFIXES bin
            NO_CACHE)

        if(NOT dll_location)
            # message(STATUS "find_library failed to find ${dll_name}, retry with file GLOB")
            file(GLOB dll_location "${search_dir}/bin/${lib_stem}*.dll")

            list(LENGTH dll_location len)

            if(len LESS_EQUAL 0)
                message(WARNING "Trying to install dll of ${lib_location}, but failed to find ${dll_name}. 
                    The released package may failed to find this dll.")
                return()
            endif()
        endif()

        SlopeCraft_install_lib(${dll_location} ${dest})

        return()
    endif()

    if(extension MATCHES ".dll")
        message(STATUS "Install ${lib_location} to ${dest}")
        install(FILES ${lib_location}
            DESTINATION ${dest})
        return()
    endif()

    if((extension MATCHES ".a") OR(extension MATCHES ".lib"))
        return()
    endif()
endfunction(SlopeCraft_install_lib)

function(SlopeCraft_install_if_is_shared target dest)
    get_target_property(target_type ${target} TYPE)

    # message(STATUS "Type of ${target} is : ${target_type}")
    get_target_property(target_name ${target} NAME)

    # message(STATUS "Name of ${target} is : ${target_name}")
    get_target_property(target_suffix ${target} SUFFIX)

    # message(STATUS "Suffix of ${target} is : ${target_suffix}")
    get_target_property(target_location ${target} LOCATION)

    # message(STATUS "Location of ${target} is : ${target_location}")
    set(skip_this false)

    if(${target_type} STREQUAL EXECUTABLE)
        set(skip_this true)
    endif()

    if(${target_type} STREQUAL STATIC_LIBRARY)
        set(skip_this true)
    endif()

    if(${target_type} STREQUAL OBJECT_LIBRARY)
        set(skip_this true)
    endif()

    if(${skip_this})
        message(STATUS "Skip installing ${target} because it is not a shared lib. TYPE = ${target_type}")
        return()
    endif()

    SlopeCraft_install_lib(${target_location} ${dest})
endfunction(SlopeCraft_install_if_is_shared)