# https://github.com/Chocobo1/Hash

set(SlopeCraft_Chocobo1_Hash_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/Hash/src)

if(EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/Hash/.git)
    return()
endif()

file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/3rdParty/Hash)

message(STATUS "cloning https://github.com/Chocobo1/Hash.git")

execute_process(COMMAND git clone https://github.com/Chocobo1/Hash.git
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty
    COMMAND_ERROR_IS_FATAL ANY)

# execute_process(git checkout )