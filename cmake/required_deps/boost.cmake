if (${WIN32})
    find_package(Boost
        COMPONENTS iostreams
        OPTIONAL_COMPONENTS multi_array
    )
else ()
    message(STATUS "Finding boost without multi_array to avoid a fucking stupid error in BoostConfig.cmake")
    find_package(Boost
        COMPONENTS iostreams
    )
endif ()

if (NOT TARGET Boost::iostreams)
    message(FATAL_ERROR "Failed to find Boost::iostreams, install boost first")
endif ()
if (NOT TARGET Boost::multi_array)
    message(WARNING "find_package failed to find Boost::multi_array, trying to find its headers manually...")
    set(hints)
    foreach (prefix ${CMAKE_PREFIX_PATH})
        list(APPEND hints "${prefix}/include/boost/multi_array.hpp")
    endforeach ()
    find_file(multi_array_hpp_location NAMES multi_array.hpp
        HINTS ${hints}
        PATH_SUFFIXES "include/boost"
    )
    unset(hpp_count)
    list(LENGTH multi_array_hpp_location hpp_count)
    if ((NOT multi_array_hpp_location) OR (${hpp_count} LESS_EQUAL 0))
        message(FATAL_ERROR "Boost::multi_array is not installed or not accessible,
        you should install it and add the installation prefix to CMAKE_PREFIX_PATH")
    endif ()

    if (${hpp_count} GREATER 1)
        message(STATUS "Found multiple installation of \"include/boost/multi_array.hpp\": \n\"${multi_array_hpp_location}\", select the first.")
        list(GET multi_array_hpp_location 0 multi_array_hpp_location)
    else ()
        message(STATUS "Manually found Boost::multi_array at \"${multi_array_hpp_location}\"")
    endif ()
    message(STATUS "Making fake target named Boost::multi_array to replace the missing one. \n
    Сука, what's wrong with BoostConfig.cmake and FindBoost.cmake? I have to write moooore code to fix it on ubuntu and mac. I don't experience such bug with vcpkg+windows")

    cmake_path(GET multi_array_hpp_location PARENT_PATH boost_dir_boost)
    cmake_path(GET boost_dir_boost PARENT_PATH boost_dir_include)

    add_library(boost_fake_target_multi_array INTERFACE)
    target_include_directories(boost_fake_target_multi_array INTERFACE ${boost_dir_include})
    add_library(Boost::multi_array ALIAS boost_fake_target_multi_array)

endif ()