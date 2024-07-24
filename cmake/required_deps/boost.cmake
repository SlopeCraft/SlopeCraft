find_package(Boost
    COMPONENTS iostreams multi_array
    REQUIRED)

if (NOT TARGET Boost::iostreams)
    message(FATAL_ERROR "Failed to find Boost::iostreams, install boost first")
endif ()
if (NOT TARGET Boost::multi_array)
    message(FATAL_ERROR "Failed to find Boost::multi_array, install boost first")
endif ()