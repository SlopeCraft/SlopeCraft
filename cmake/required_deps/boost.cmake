find_package(Boost
    COMPONENTS iostreams
    REQUIRED)

if (NOT TARGET Boost::iostreams)
    message(FATAL_ERROR "Failed to find Boost::iostreams, install boost first")
endif ()