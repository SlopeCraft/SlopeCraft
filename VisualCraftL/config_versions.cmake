# set(VisualCraftL_version_string ${SlopeCraft_version})

configure_file(version.cpp.in version.cpp)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(VisualCraftL.rc.in VisualCraftL.rc)
endif()