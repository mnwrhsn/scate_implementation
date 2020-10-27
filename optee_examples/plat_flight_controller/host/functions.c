#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_flight_controller_ta.h>


/* include PCA header */
#include "PCA9685.h"

/* include helper functions */
#include "functions.h"

/* include pid functions */
#include "pid_functions.c"   // for readibility use seperate file


void flc_job_vanilla() {

    /* servo initialization */
	PCA9685 pwm;
	init_pwm(&pwm);


    // 1. First, read raw values from MPU-6050
    readSensor();

    // 2. Calculate angles from gyro & accelerometer's values
    calculateAngles();

    // 3. Calculate set points of PID controller
    calculateSetPoints();

    // 4. Calculate errors comparing angular motions to set points
    calculateErrors();

    // 5. Calculate motors speed with PID controller
    isStarted();
    pidController();
    compensateBatteryDrop();
    
    // 6. Apply motors speed
    applyMotorSpeed_vanilla(&pwm);

}


void flc_job_fine_grain() {

    /* servo initialization */
	PCA9685 pwm;
	init_pwm(&pwm);


    // 1. First, read raw values from MPU-6050
    readSensor();

    // 2. Calculate angles from gyro & accelerometer's values
    calculateAngles();

    // 3. Calculate set points of PID controller
    calculateSetPoints();

    // 4. Calculate errors comparing angular motions to set points
    calculateErrors();

    // 5. Calculate motors speed with PID controller
    isStarted();
    pidController();
    compensateBatteryDrop();
    
    // 6. Apply motors speed
    applyMotorSpeed_fine_grain(&pwm);

}



void flc_job_intermittent() {

    /* servo initialization */
	PCA9685 pwm;
	init_pwm(&pwm);


    // 1. First, read raw values from MPU-6050
    readSensor();

    // 2. Calculate angles from gyro & accelerometer's values
    calculateAngles();

    // 3. Calculate set points of PID controller
    calculateSetPoints();

    // 4. Calculate errors comparing angular motions to set points
    calculateErrors();

    // 5. Calculate motors speed with PID controller
    isStarted();
    pidController();
    compensateBatteryDrop();
    
    // 6. Apply motors speed
    applyMotorSpeed_intermittent(&pwm);

}



