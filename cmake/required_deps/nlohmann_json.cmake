
set(SlopeCraft_njson_hints)
foreach (prefix ${CMAKE_PREFIX_PATH})
    list(APPEND SlopeCraft_njson_hints "${prefix}/include/nlohmann/json.hpp")
endforeach ()

#message(STATUS "hints = ${SlopeCraft_njson_hints}")

find_file(nlohmann_json_location
    NAMES json.hpp
    HINTS ${SlopeCraft_njson_hints} "/usr/include/nlohmann/json.hpp"
    PATH_SUFFIXES nlohmann
    PATHS ${CMAKE_PREFIX_PATH} "/usr" "/usr/local"
    NO_CACHE
)
#message(STATUS "nlohmann_json_location = ${nlohmann_json_location}")

if (NOT nlohmann_json_location)
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "nlohmann json is not found. SlopeCraft is not allowed to download during configuring, please install it and try again")
    endif ()

    set(nlohmann_json_location ${CMAKE_BINARY_DIR}/3rdParty/nlohmann/json.hpp)
    message(STATUS "Downloading nlohmann json...")
    file(DOWNLOAD
        "https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp"
        ${CMAKE_BINARY_DIR}/3rdParty/nlohmann/json.hpp
        EXPECTED_HASH SHA256=aaf127c04cb31c406e5b04a63f1ae89369fccde6d8fa7cdda1ed4f32dfc5de63
    )
endif ()

message(STATUS "nlohmann json found/downloaded at ${nlohmann_json_location}")

cmake_path(GET nlohmann_json_location PARENT_PATH SlopeCraft_nlohmann_json_include_dir)
