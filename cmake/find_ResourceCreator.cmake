# https://github.com/isRyven/ResourceCreator.cmake.git
set(SlopeCraft_rc_creator_found OFF)

if(EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/ResourceCreator.cmake/.git)
    set(SlopeCraft_rc_creator_found TRUE)
    return()
endif()

message(STATUS "Cloning ResourceCreator.cmake...")
execute_process(COMMAND git clone "https://github.com/isRyven/ResourceCreator.cmake.git"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty
    COMMAND_ERROR_IS_FATAL ANY
)
set(SlopeCraft_rc_creator_found TRUE)