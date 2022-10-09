
set(SlopeCraft_download_Eigen340 OFF)

if(NOT DEFINED SlopeCraft_Eigen3_include_dir)
    # try to find Eigen3 by find_package
    find_package(Eigen3 3.4)
    if(${EIGEN3_FOUND})
        message(STATUS "CMake have found an libEigen by find_package")
        set(SlopeCraft_Eigen3_include_dir ${EIGEN3_INCLUDE_DIR})
    else()
        # try to find Eigen3 in the 3rdParty directory
        if(EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/eigen/Eigen/Dense)
            message(STATUS "Found Eigen in 3rdParty/eigen")
            set(SlopeCraft_Eigen3_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/eigen)
        else()
            set(SlopeCraft_download_Eigen340 ON)
        endif()
    endif()

else()
    # examine whether the input is valid
    if(EXISTS ${SlopeCraft_Eigen3_include_dir}/Eigen/Dense)
        # the assigned Eigen3_include_dir is valid. nothing to do.
        message("The value of SlopeCraft_Eigen3_include_dir is valid. SlopeCraft will have access to Eigen3")
    else()
        message(WARNING 
            "The value of SlopeCraft_Eigen3_include_dir is invalid. Failed to find file : "
            ${SlopeCraft_Eigen3_include_dir}/Eigen/Dense)
        set(SlopeCraft_download_Eigen340 ON)
    endif()
endif()

if(${SlopeCraft_download_Eigen340})
    message(STATUS "Cloning Eigen...")
    execute_process(
        COMMAND git clone https://gitlab.com/libeigen/eigen.git
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty
        COMMAND_ERROR_IS_FATAL ANY
        ECHO_ERROR_VARIABLE
        ECHO_OUTPUT_VARIABLE
    )

    message(STATUS "Successfully cloned Eigen. Checking out to tag 3.4.0")
    execute_process(
        COMMAND "git checkout 3.4.0"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdParty/eigen
        COMMAND_ERROR_IS_FATAL ANY
        ECHO_ERROR_VARIABLE
        ECHO_OUTPUT_VARIABLE
    )
    message(STATUS "Eigen v3.4.0 is ready")

    set(SlopeCraft_download_Eigen340 OFF)
    set(SlopeCraft_Eigen3_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/eigen)
endif()