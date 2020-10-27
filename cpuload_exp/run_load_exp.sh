#!/bin/bash

timeout_duration="60s"
num_exp="10"


# change filename field based on what function you compiled in optee_examples/plat_X/host/main.c
# filename are: load_<platform>_<suffix> --> suffix: vanilla, fg, ic

# filename="CPU_USE_ROBOT_ARM_VANILLA.txt"
# filename="CPU_USE_ROBOT_ARM_FG.txt"
# filename="CPU_USE_ROBOT_ARM_IC.txt"
# platform="robot_arm"

# filename="CPU_USE_SYRINGE_PUMP_VANILLA.txt"
# filename="CPU_USE_SYRINGE_PUMP_FG.txt"
# filename="CPU_USE_SYRINGE_PUMP_IC.txt"
# platform="syringe_pump"

# filename="CPU_USE_FLIGHT_CONTROLLER_VANILLA.txt"
# filename="CPU_USE_FLIGHT_CONTROLLER_FG.txt"
# filename="CPU_USE_FLIGHT_CONTROLLER_IC.txt"
# platform="flight_controller"

filename="CPU_USE_ROVER_VANILLA.txt"
# filename="CPU_USE_ROVER_FG.txt"
# filename="CPU_USE_ROVER_IC.txt"
platform="rover"

# remove old files if any
sudo rm "$filename"
echo "running load experiment for $timeout_duration. platform: $platform, filename: $filename"
for ((n=1;n<$num_exp;n++))
do

    echo "===================="
    echo "Experiment No. # $n"
    echo "===================="

    timeout $timeout_duration ./do_run_load_exp.sh $filename

    # take some rest
    sleep 5  


done


echo "==== EXPERIMENT DONE! ===="


