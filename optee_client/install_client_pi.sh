#!/bin/sh


# run this inside Raspberry Pi (use Sudo)

make clean
cmake .
make
sudo make install
# sudo rm /bin//tee-supplicant


# ROOTDIR="$(pwd)"

# sudo cp -a $ROOTDIR/libteec/libteec.so* /lib
# # sudo cp -a $ROOTDIR/libteec/libteec.a /lib
# sudo cp $ROOTDIR/tee-supplicant/tee-supplicant /bin
# sudo cp $ROOTDIR/public/*.h /include

echo "Script finished!"
