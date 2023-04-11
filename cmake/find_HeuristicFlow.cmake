set(SlopeCraft_download_HeuristicFlow OFF)

if(NOT DEFINED SlopeCraft_HeuristicFlow_include_dir)
    # try to find Heu in the 3rdParty directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/HeuristicFlow/HeuristicFlow/Genetic)
        message(STATUS "Found HeuristicFlow in 3rdParty/HeusiticFlow")
        set(SlopeCraft_HeuristicFlow_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/HeuristicFlow)

    else()
        set(SlopeCraft_download_HeuristicFlow ON)
    endif()

else()
    #examine whether the input is valid
    if(EXISTS ${SlopeCraft_HeuristicFlow_include_dir}/HeuristicFlow/Genetic)
        # is valid
        message("The value of SlopeCraft_HeuristicFlow_include_dir is valid. SlopeCraft will have access to Heu")

    else()
        message(WARNING 
            "The value of SlopeCraft_HeuristicFlow_include_dir is invalid. Failed to find file : "
            ${SlopeCraft_HeuristicFlow_include_dir}/HeuristicFlow/Genetic)
        set(SlopeCraft_download_HeuristicFlow ON)
    endif()
endif()


# download
if(${SlopeCraft_download_HeuristicFlow})
    #if()
    message(STATUS "Cloning HeuristicFlow...")
    execute_process(
        COMMAND git clone https://github.com/SlopeCraft/HeuristicFlow.git
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty
        COMMAND_ERROR_IS_FATAL ANY
        ECHO_ERROR_VARIABLE
        ECHO_OUTPUT_VARIABLE
    )

    message(STATUS "Successfully cloned HeuristicFlow. Checking out to tag v1.6.2.2")
    execute_process(
        COMMAND git checkout v1.6.2.2
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty/HeuristicFlow
        COMMAND_ERROR_IS_FATAL ANY
        ECHO_ERROR_VARIABLE
    )

    message(STATUS "HeuristicFlow is ready")

    set(SlopeCraft_download_HeuristicFlow OFF)
    set(SlopeCraft_HeuristicFlow_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/HeuristicFlow)

endif()
