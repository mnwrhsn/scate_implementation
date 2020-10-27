
Scripts in this directoy will be used to run cpu load experiments.

* first get this directory using SVN trunk (see main readme file)
* compile `cpuusage.c`: `gcc cpuusage.c -o cpu`
* use `scp_ca_ta.sh` to copy TA/CA files from development machine (Ubuntu VM in macOS in my setup)
* run `sudo timeout <time_in_sec> ./run_load_exp.sh` to obtain the CPU loads.
* alternatively, check `do_run_load_exp.sh` for automated scripts
* see `run_load_exp.sh` to add/remove experiments
* results will be save in the current directory (in text files)
* make sure the corresponding TA/CA files are compiled for CPU load experiments. See `main.c` in the correspoding file.
(say `/optee_examples/plat_robot_arm/host/main.c`)
