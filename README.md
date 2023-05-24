### Implementation of SCATE (ISORC'23)

**Paper Link: https://monowarhasan.info/papers/SCATE_ISORC23.pdf**

#### Clone the repo (in the host machine):

* tested on Ubuntu 16.04 LTS

` git clone --recurse-submodules https://github.com/mnwrhsn/rt_sec_io_tz.git`

#### Build and Copy TrustZone codes to Raspberry Pi:

1. Download toolchains for build:

```
cd /build
make -j2 toolchains
```

2. Now Make:

```
make -j `nproc`
```

3. Make Client, examples, etc. (this is required since we use a different Makefile -- see `/build/Makefile`):

```
make optee-os optee-client optee-examples
```

See the details in OPTEE Guide: https://optee.readthedocs.io/building/gits/build.html#build

3. Check `copy_to_sdcard.sh` for details of copying the compiled code to Raspberry Pi

4. Locally bulid TEE-Supplicant (due to glibc compatibility) 

[this step needs to be performed in RPi3. Install necessary packages as required]

Download the optee-client:
`svn export https://github.com/mnwrhsn/rt_sec_io_tz/trunk/optee_client`

first `cd` to `optee_client`
and then:
```
make clean
cmake .
make
sudo make install
```

5. In the RPi3 -> load supplicant: `sudo tee-supplicant &`

**For Cross Compiling:** use the script `scp_ca_ta.sh` to copy files from host machine to PI [must be running on RPi] 

**Update (July 2020)**
use the following to clone cpuload_exp directory in the RPi

`svn export https://github.com/mnwrhsn/rt_sec_io_tz/branches/van_lin_ker/cpuload_exp`

----

### How to Add new TA:

* To add a new TA: create directory with proper name in `optee_examples`
* Modify main `Makefile` in the `optee_examples/Makefile` directory --> check `prepare-for-rootfs` flag and add your TA to be copied in the `optee-examples/out/` directory
* Say our new TA name is `plat_rover`. Do the following changes:
  - Add `plat_rover` direcotry in `optee_exaples`
  - Use Similar `host` and `TA` direcotry layout as in `optee_exampels/hello_world`
  - Modify `.gitignore` in `optee_examples` to untract your `plat_rover` binary
  - Modify Line 16 in `optee_examples/plat_rover/Android.mk`
  - Modify Line 1 in `optee_examples/plat_rover/CMakeLists.txt`
  - Modify Line 2 in `optee_examples/plat_rover/ta/Android.mk` with new UUID
  - Modify Line 5 in `optee_examples/plat_rover/ta/Makefile` with new UUID
  - Modify Line 2 in `optee_examples/plat_rover/ta/sub.mk` with the source TA name
  - Modify Line 15 in `optee_examples/plat_rover/host/Makefile` with the updated TA name
  - Modify TA header file name in `optee_examples/plat_rover/ta/include/` and also use proper TA filenames in both `host` and `TA` directory
