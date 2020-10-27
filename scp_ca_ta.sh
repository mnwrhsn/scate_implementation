#!/bin/bash

# NOTE: THIS SHOULD BE RUNNING ON RPI  #

OPTEE_ROOT="/home/mhasan/workspace/rt_sec_io_tz"
USER_NAME="mhasan"
IP="192.168.200.11"

echo "Copying CA binaries..."

LOC="$OPTEE_ROOT/optee_examples/out/ca/*"
sudo scp -r $USER_NAME@$IP:$LOC  /bin/

echo "Copy TA files.."

LOC="$OPTEE_ROOT/optee_examples/out/ta/*"
sudo scp -r $USER_NAME@$IP:$LOC /lib/optee_armtz/

echo "SCP finished!"
