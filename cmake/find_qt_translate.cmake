# The executable to generate .qm files from .qm files
set(SlopeCraft_Qt_lrelease_executable ${SlopeCraft_Qt_root_dir}/bin/lrelease)
#message("Testing lrelease : ")
message("The executable of lrelease is : " ${SlopeCraft_Qt_lrelease_executable})
execute_process(COMMAND ${SlopeCraft_Qt_lrelease_executable} -version
    OUTPUT_VARIABLE SlopeCraft_output_variable)
#message("The output is : " ${SlopeCraft_output_variable})

# The executable to update .ts files
set(SlopeCraft_Qt_lupdate_executable ${SlopeCraft_Qt_root_dir}/bin/lupdate)
#message("Testing lupdate : ")
message("The executable of lupdate is : " ${SlopeCraft_Qt_lupdate_executable})
execute_process(COMMAND ${SlopeCraft_Qt_lupdate_executable} -version
    OUTPUT_VARIABLE SlopeCraft_output_variable)
#message("The output is : " ${SlopeCraft_output_variable})