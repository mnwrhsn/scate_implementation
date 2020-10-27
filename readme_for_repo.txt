# create git repo with for OPTEE-RPi3 (V3.4.0)



1. Follow build instruction in OPTEE site
2. Once downloaded, remove internal .git files:

( find . -type d -name ".git" \
  && find . -name ".gitmodules" ) | xargs rm -rf
  
3. Remove the following directory:

* arm-trusted-firmware
* buildroot     
* firmware
* u-boot

sudo rm -r arm-trusted-firmware buildroot firmware u-boot

4. Add this directory as submodules

git submodule add https://github.com/ARM-software/arm-trusted-firmware.git            
git submodule add https://github.com/buildroot/buildroot.git            
git submodule add https://github.com/raspberrypi/firmware.git
git submodule add https://github.com/u-boot/u-boot.git          



5. For each of the directory in Step 5, checkout to specific branch as in manifest_rpi3.xml         

For example,

cd arm-trusted-firmware; git checkout 023bc019e95ca98687f015074c938941a0546eb7; cd ..

cd buildroot; git checkout  refs/tags/2018.08; cd ..

cd firmware; git checkout refs/tags/1.20170215; cd ..

cd u-boot; git checkout aac0c29d4b8418c5c78b552070ffeda022b16949; cd ..



