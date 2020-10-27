
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <stdbool.h>  //boolean


/* include PCA header */
#include "PCA9685.h"


/* include PID related globals */
#include "pid_global.h"



/* 
    PID controller code from:
    https://github.com/lobodol/drone-flight-controller
*/

void do_i2c_read(unsigned char* buffer, int length) {

    int file_i2c;

	/* ----- OPEN THE I2C BUS ----- */
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open the i2c bus\n");
		return;
	}
	
	int addr = 0x6F;          // I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		return;
	}
	
	/* ----- READ BYTES ----- */
	if (read(file_i2c, buffer, length) != length)		
	{
		printf("Failed to read from the i2c bus.\n");
	}

    close(file_i2c);  // close handler
	
    return;
}


void do_i2c_write(unsigned char* buffer, int length) {

    int file_i2c;

	/* ----- OPEN THE I2C BUS ----- */
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open the i2c bus\n");
		return;
	}
	
	int addr = 0x6F;          // I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		return;
	}
	
    /* ----- WRITE BYTES ----- */
	if (write(file_i2c, buffer, length) != length)		
	{
		printf("Failed to write to the i2c bus.\n");
        return;
	}

    close(file_i2c);  // close handler
	
    return;
}

/**
 * Make sure that given value is not over min_value/max_value range.
 *
 * @param float value     : The value to convert
 * @param float min_value : The min value
 * @param float max_value : The max value
 *
 * @return float
 */
float minMax(float value, float min_value, float max_value) {
    if (value > max_value) {
        value = max_value;
    } else if (value < min_value) {
        value = min_value;
    }

    return value;
}


/**
 * Request raw values from MPU6050.
 */
void readSensor() {
    
    // simulate sensor readings
    unsigned char buffer[60] = {0};
    do_i2c_read(buffer, 14);   // Request 14 bytes from the MPU-6050
    usleep(BCM_DELAY);  // wait a bit

    int rval;  // a random variable to simulate analog (raw) sensor reading. 1 byte value [0, 255]

    rval = rand() % 256;  // random number in range (0, 255)
    acc_raw[X]  = rval << 8 | rval; // Add the low and high byte to the acc_raw[X] variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    acc_raw[Y]  = rval << 8 | rval; // Add the low and high byte to the acc_raw[Y] variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    acc_raw[Z]  = rval << 8 | rval; // Add the low and high byte to the acc_raw[Z] variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    temperature = rval << 8 | rval; // Add the low and high byte to the temperature variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    gyro_raw[X] = rval << 8 | rval; // Add the low and high byte to the gyro_raw[X] variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    gyro_raw[Y] = rval << 8 | rval; // Add the low and high byte to the gyro_raw[Y] variable
    
    rval = rand() % 256;  // random number in range (0, 255)
    gyro_raw[Z] = rval << 8 | rval; // Add the low and high byte to the gyro_raw[Z] variable
    

}

/**
 * Calculate pitch & roll angles using only the gyro.
 */
void calculateGyroAngles() {
    // Subtract offsets
    gyro_raw[X] -= gyro_offset[X];
    gyro_raw[Y] -= gyro_offset[Y];
    gyro_raw[Z] -= gyro_offset[Z];

    // Angle calculation using integration
    gyro_angle[X] += (gyro_raw[X] / (FREQ * SSF_GYRO));
    gyro_angle[Y] += (-gyro_raw[Y] / (FREQ * SSF_GYRO)); // Change sign to match the accelerometer's one

    // Transfer roll to pitch if IMU has yawed
    gyro_angle[Y] += gyro_angle[X] * sin(gyro_raw[Z] * (PI / (FREQ * SSF_GYRO * 180)));
    gyro_angle[X] -= gyro_angle[Y] * sin(gyro_raw[Z] * (PI / (FREQ * SSF_GYRO * 180)));
}

/**
 * Calculate pitch & roll angles using only the accelerometer.
 */
void calculateAccelerometerAngles() {
    // Calculate total 3D acceleration vector : √(X² + Y² + Z²)
    acc_total_vector = sqrt(pow(acc_raw[X], 2) + pow(acc_raw[Y], 2) + pow(acc_raw[Z], 2));

    // To prevent asin to produce a NaN, make sure the input value is within [-1;+1]
    if (abs(acc_raw[X]) < acc_total_vector) {
        acc_angle[X] = asin((float)acc_raw[Y] / acc_total_vector) * (180 / PI); // asin gives angle in radian. Convert to degree multiplying by 180/pi
    }

    if (abs(acc_raw[Y]) < acc_total_vector) {
        acc_angle[Y] = asin((float)acc_raw[X] / acc_total_vector) * (180 / PI);
    }
}

/**
 * Reset gyro's angles with accelerometer's angles.
 */
void resetGyroAngles() {
    gyro_angle[X] = acc_angle[X];
    gyro_angle[Y] = acc_angle[Y];
}

/**
 * Reset motors' pulse length to 1000µs to totally stop them.
 */
void stopAll() {
    pulse_length_esc1 = 1000;
    pulse_length_esc2 = 1000;
    pulse_length_esc3 = 1000;
    pulse_length_esc4 = 1000;
}

/**
 * Reset all PID controller's variables.
 */
void resetPidController() {
    errors[YAW]   = 0;
    errors[PITCH] = 0;
    errors[ROLL]  = 0;

    error_sum[YAW]   = 0;
    error_sum[PITCH] = 0;
    error_sum[ROLL]  = 0;

    previous_error[YAW]   = 0;
    previous_error[PITCH] = 0;
    previous_error[ROLL]  = 0;
}



/**
 * Calculate real angles from gyro and accelerometer's values
 */
void calculateAngles() {
    calculateGyroAngles();
    calculateAccelerometerAngles();

    if (initialized) {
        // Correct the drift of the gyro with the accelerometer
        gyro_angle[X] = gyro_angle[X] * 0.9996 + acc_angle[X] * 0.0004;
        gyro_angle[Y] = gyro_angle[Y] * 0.9996 + acc_angle[Y] * 0.0004;
    } else {
        // At very first start, init gyro angles with accelerometer angles
        resetGyroAngles();

        initialized = true;
    }

    // To dampen the pitch and roll angles a complementary filter is used
    measures[ROLL]  = measures[ROLL]  * 0.9 + gyro_angle[X] * 0.1;
    measures[PITCH] = measures[PITCH] * 0.9 + gyro_angle[Y] * 0.1;
    measures[YAW]   = -gyro_raw[Z] / SSF_GYRO; // Store the angular motion for this axis

    // Apply low-pass filter (10Hz cutoff frequency)
    angular_motions[ROLL]  = 0.7 * angular_motions[ROLL]  + 0.3 * gyro_raw[X] / SSF_GYRO;
    angular_motions[PITCH] = 0.7 * angular_motions[PITCH] + 0.3 * gyro_raw[Y] / SSF_GYRO;
    angular_motions[YAW]   = 0.7 * angular_motions[YAW]   + 0.3 * gyro_raw[Z] / SSF_GYRO;
}

/**
 * Calculate the PID set point in °/s
 *
 * @param float angle         Measured angle (in °) on an axis
 * @param int   channel_pulse Pulse length of the corresponding receiver channel
 * @return float
 */
float calculateSetPoint(float angle, int channel_pulse) {
    float level_adjust = angle * 15; // Value 15 limits maximum angle value to ±32.8°
    float set_point    = 0;

    // Need a dead band of 16µs for better result
    if (channel_pulse > 1508) {
        set_point = channel_pulse - 1508;
    } else if (channel_pulse <  1492) {
        set_point = channel_pulse - 1492;
    }

    set_point -= level_adjust;
    set_point /= 3;

    return set_point;
}

/**
 * Calculate the PID set point of YAW axis in °/s
 *
 * @param int yaw_pulse      Receiver pulse length of yaw's channel
 * @param int throttle_pulse Receiver pulse length of throttle's channel
 * @return float
 */
float calculateYawSetPoint(int yaw_pulse, int throttle_pulse) {
    float set_point = 0;

    // Do not yaw when turning off the motors
    if (throttle_pulse > 1050) {
        // There is no notion of angle on this axis as the quadcopter can turn on itself
        set_point = calculateSetPoint(0, yaw_pulse);
    }

    return set_point;
}


/**
 * Calculate PID set points on axis YAW, PITCH, ROLL
 */
void calculateSetPoints() {
    pid_set_points[YAW]   = calculateYawSetPoint(pulse_length[mode_mapping[YAW]], pulse_length[mode_mapping[THROTTLE]]);
    pid_set_points[PITCH] = calculateSetPoint(measures[PITCH], pulse_length[mode_mapping[PITCH]]);
    pid_set_points[ROLL]  = calculateSetPoint(measures[ROLL], pulse_length[mode_mapping[ROLL]]);
}


/**
 * Calculate errors used by PID controller
 */
void calculateErrors() {
    // Calculate current errors
    errors[YAW]   = angular_motions[YAW]   - pid_set_points[YAW];
    errors[PITCH] = angular_motions[PITCH] - pid_set_points[PITCH];
    errors[ROLL]  = angular_motions[ROLL]  - pid_set_points[ROLL];

    // Calculate sum of errors : Integral coefficients
    error_sum[YAW]   += errors[YAW];
    error_sum[PITCH] += errors[PITCH];
    error_sum[ROLL]  += errors[ROLL];

    // Keep values in acceptable range
    error_sum[YAW]   = minMax(error_sum[YAW],   -400/Ki[YAW],   400/Ki[YAW]);
    error_sum[PITCH] = minMax(error_sum[PITCH], -400/Ki[PITCH], 400/Ki[PITCH]);
    error_sum[ROLL]  = minMax(error_sum[ROLL],  -400/Ki[ROLL],  400/Ki[ROLL]);

    // Calculate error delta : Derivative coefficients
    delta_err[YAW]   = errors[YAW]   - previous_error[YAW];
    delta_err[PITCH] = errors[PITCH] - previous_error[PITCH];
    delta_err[ROLL]  = errors[ROLL]  - previous_error[ROLL];

    // Save current error as previous_error for next time
    previous_error[YAW]   = errors[YAW];
    previous_error[PITCH] = errors[PITCH];
    previous_error[ROLL]  = errors[ROLL];
}

/**
 * Return whether the quadcopter is started.
 * To start the quadcopter, move the left stick in bottom left corner then, move it back in center position.
 * To stop the quadcopter move the left stick in bottom right corner.
 *
 * @return bool
 */
bool isStarted() {
    // When left stick is moved in the bottom left corner
    if (status == STOPPED && pulse_length[mode_mapping[YAW]] <= 1012 && pulse_length[mode_mapping[THROTTLE]] <= 1012) {
        status = STARTING;
    }

    // When left stick is moved back in the center position
    if (status == STARTING && pulse_length[mode_mapping[YAW]] == 1500 && pulse_length[mode_mapping[THROTTLE]] <= 1012) {
        status = STARTED;

        // Reset PID controller's variables to prevent bump start
        resetPidController();

        resetGyroAngles();
    }

    // When left stick is moved in the bottom right corner
    if (status == STARTED && pulse_length[mode_mapping[YAW]] >= 1988 && pulse_length[mode_mapping[THROTTLE]] <= 1012) {
        status = STOPPED;
        // Make sure to always stop motors when status is STOPPED
        stopAll();
    }

    return status == STARTED;
}


/**
 * Calculate motor speed for each motor of an X quadcopter depending on received instructions and measures from sensor
 * by applying PID control.
 *
 * (A) (B)     x
 *   \ /     z ↑
 *    X       \|
 *   / \       +----→ y
 * (C) (D)
 *
 * Motors A & D run clockwise.
 * Motors B & C run counter-clockwise.
 *
 * Each motor output is considered as a servomotor. As a result, value range is about 1000µs to 2000µs
 */
void pidController() {
    float yaw_pid      = 0;
    float pitch_pid    = 0;
    float roll_pid     = 0;
    int   throttle     = pulse_length[mode_mapping[THROTTLE]];

    // Initialize motor commands with throttle
    pulse_length_esc1 = throttle;
    pulse_length_esc2 = throttle;
    pulse_length_esc3 = throttle;
    pulse_length_esc4 = throttle;

    // Do not calculate anything if throttle is 0
    if (throttle >= 1012) {
        // PID = e.Kp + ∫e.Ki + Δe.Kd
        yaw_pid   = (errors[YAW]   * Kp[YAW])   + (error_sum[YAW]   * Ki[YAW])   + (delta_err[YAW]   * Kd[YAW]);
        pitch_pid = (errors[PITCH] * Kp[PITCH]) + (error_sum[PITCH] * Ki[PITCH]) + (delta_err[PITCH] * Kd[PITCH]);
        roll_pid  = (errors[ROLL]  * Kp[ROLL])  + (error_sum[ROLL]  * Ki[ROLL])  + (delta_err[ROLL]  * Kd[ROLL]);

        // Keep values within acceptable range. TODO export hard-coded values in variables/const
        yaw_pid   = minMax(yaw_pid, -400, 400);
        pitch_pid = minMax(pitch_pid, -400, 400);
        roll_pid  = minMax(roll_pid, -400, 400);

        // Calculate pulse duration for each ESC
        pulse_length_esc1 = throttle - roll_pid - pitch_pid + yaw_pid;
        pulse_length_esc2 = throttle + roll_pid - pitch_pid - yaw_pid;
        pulse_length_esc3 = throttle - roll_pid + pitch_pid - yaw_pid;
        pulse_length_esc4 = throttle + roll_pid + pitch_pid + yaw_pid;
    }

    // Prevent out-of-range-values
    pulse_length_esc1 = minMax(pulse_length_esc1, 1100, 2000);
    pulse_length_esc2 = minMax(pulse_length_esc2, 1100, 2000);
    pulse_length_esc3 = minMax(pulse_length_esc3, 1100, 2000);
    pulse_length_esc4 = minMax(pulse_length_esc4, 1100, 2000);

    // printf("Pulse length %d %d %d %d...\n", pulse_length_esc1, pulse_length_esc2, pulse_length_esc3, pulse_length_esc4);

    // mhasan add: run a busy-loop
    for (unsigned i = 0; i < 1000; i++)
        __asm__ __volatile__ ("" : "+g" (i) : :);
    

}


/**
 * Read battery voltage & return whether the battery seems connected
 *
 * @return boolean
 */
bool isBatteryConnected() {
    // Reduce noise with a low-pass filter (10Hz cutoff frequency)

    //battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;

    // mhasan add
    unsigned char buffer[60] = {0};
    do_i2c_read(buffer, 10);   // Request 10 bytes 
    int analogRead = rand() % 1024;  // 10-bit analog read. see https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
    battery_voltage = battery_voltage * 0.92 + (analogRead + 65) * 0.09853;
    
    return battery_voltage < 1240 && battery_voltage > 800;
}

/**
 * Compensate battery drop applying a coefficient on output values
 */
void compensateBatteryDrop() {
    if (isBatteryConnected()) {
        // printf("battery connected!\n");
        pulse_length_esc1 += pulse_length_esc1 * ((1240 - battery_voltage) / (float) 3500);
        pulse_length_esc2 += pulse_length_esc2 * ((1240 - battery_voltage) / (float) 3500);
        pulse_length_esc3 += pulse_length_esc3 * ((1240 - battery_voltage) / (float) 3500);
        pulse_length_esc4 += pulse_length_esc4 * ((1240 - battery_voltage) / (float) 3500);
    }
}

// obtain from https://github.com/thedch/quadcopter/blob/master/motors/motors.py
int calcTicks(double hz, unsigned long pulse_mus) {
    double cycle = 1000 / hz;
	double timePerTick = cycle / 4096;
    unsigned long pulseInMilliseconds =  0.001 * pulse_mus;
	int ticks = (int) (pulseInMilliseconds / timePerTick);
	return ticks;
}
	
// mhasan add: initialize PCA

void init_pwm(PCA9685* pwm) {
    pwm->init(1,0x6F);
	usleep(PWM_SLEEP_TIME);
	// pwm->setPWMFreq(PWM_FREQUENCY);
	// usleep(PWM_SLEEP_TIME);
}

/**
 * Generate servo-signal on digital pins #4 #5 #6 #7 with a frequency of 250Hz (4ms period).
 * Direct port manipulation is used for performances.
 *
 * This function might not take more than 2ms to run, which lets 2ms remaining to do other stuff.
 *
 * @see https:// www.arduino.cc/en/Reference/PortManipulation
 */
void applyMotorSpeed_vanilla(PCA9685* pwm) {
    
    /* NOTE: modified by mhasan for RPi with PWM servo controllers */

    // printf("tick val (Channel 1,2,3,4): %d, %d %d %d...\n", 
    //                 calcTicks(PWM_FREQUENCY, pulse_length_esc1),
    //                 calcTicks(PWM_FREQUENCY, pulse_length_esc2),
    //                 calcTicks(PWM_FREQUENCY, pulse_length_esc3),
    //                 calcTicks(PWM_FREQUENCY, pulse_length_esc4));

    pwm->setPWMFreq(PWM_FREQUENCY);
	usleep(PWM_SLEEP_TIME);
    
    pwm->setPWM(CHANNEL1, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc1));
    usleep(PWM_SLEEP_TIME);
    pwm->setPWM(CHANNEL2, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc2));
    usleep(PWM_SLEEP_TIME);
    pwm->setPWM(CHANNEL3, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc3));
    usleep(PWM_SLEEP_TIME);
    pwm->setPWM(CHANNEL4, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc4));
    usleep(PWM_SLEEP_TIME);

}


void _TEE_VETTED_SET_FREQ(PCA9685* pwm, int frequency) {

    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_FLIGHT_CONTROLLER_UUID;
	uint32_t err_origin;
	
	res = TEEC_InitializeContext(NULL, &ctx);  /* Initialize a context connecting us to the TEE */
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
	
	
	memset(&op, 0, sizeof(op));  /* Clear the TEEC_Operation struct */
	
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	
    op.params[0].value.a = frequency;
	
	// printf("Invoking TA to check %d\n", op.params[0].value.a);
	
    res = TEEC_InvokeCommand(&sess, TA_PLAT_FLIGHT_CONTROLLER_CHECK_FREQ, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	
    int _teeout= (int) op.params[0].value.a;  // save value returned by TEE
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

    // printf("TA returned %d\n", _teeout);

    // TEE verified OK
    if (_teeout >=0) {
        pwm->setPWMFreq(frequency);
	    usleep(PWM_SLEEP_TIME);
    }
    else {
        printf("TEE Verification failed while setting PWM frequency!\n"); // print an error message
    }
	

}


void _TEE_VETTED_SET_PULSE_LENGTH(PCA9685* pwm, int channel, unsigned long pulse_length) {

    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_FLIGHT_CONTROLLER_UUID;
	uint32_t err_origin;
	
	res = TEEC_InitializeContext(NULL, &ctx);  /* Initialize a context connecting us to the TEE */
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
	
	
	memset(&op, 0, sizeof(op));  /* Clear the TEEC_Operation struct */
	
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	
    op.params[0].value.a = channel;
    // op.params[0].value.a = pulse_length;
    int ticks = calcTicks(PWM_FREQUENCY, pulse_length);
    op.params[0].value.b = ticks;
	
	// printf("Invoking TA to check %d\n", op.params[0].value.a);
	
    res = TEEC_InvokeCommand(&sess, TA_PLAT_FLIGHT_CONTROLLER_CHECK_MOT_SPEED, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	
    int _teeout[2]  = {0};
    _teeout[0]= (int) op.params[0].value.a;  // save value returned by TEE [Channel]
    _teeout[1]= (int) op.params[0].value.b;  // save value returned by TEE [Pulse Value]
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

    // printf("TA returned <channel, value>: <%d,%d>\n", _teeout[0], _teeout[1]);

    // TEE verified OK
    if (_teeout[0] >=0 && _teeout[1] >= 0) {
        pwm->setPWM(channel, 0, ticks);
	    usleep(PWM_SLEEP_TIME);
    }
    else {
        printf("TEE Verification failed while setting PWM pulse length!\n"); // print an error message
    }
	

}



void get_action_by_indx(int* action_set, int indx) { 

    // printf("Indx is %d\n", indx);
   
    switch(indx) {
        case 0:
            action_set[0] = 1;
            action_set[1] = 1;
            break;
        case 1:
            action_set[0] = 1;
            action_set[2] = 1;
            break;
        case 2:
            action_set[0] = 1;
            action_set[3] = 1;
            break;
        case 3:
            action_set[0] = 1;
            action_set[4] = 1;
            break;
        case 4:
            action_set[0] = 1;
            action_set[5] = 1;
            break;
        case 5:
            action_set[1] = 1;
            action_set[2] = 1;
            break;
        case 6:
            action_set[1] = 1;
            action_set[3] = 1;
            break;
        case 7:
            action_set[1] = 1;
            action_set[4] = 1;
            break;
        case 8:
            action_set[2] = 1;
            action_set[3] = 1;
            break;
        case 9:
            action_set[2] = 1;
            action_set[4] = 1;
            break;
        case 10:
            action_set[3] = 1;
            action_set[4] = 1;
            break;
        default:
            // do nothing
            printf("Not matching any index. Returning...\n");
        
   }

} 

void get_rand_command_seq(int* action_set) {

    double r = (double)rand() / (double)((unsigned)RAND_MAX + 1);
    // printf("r is %lf\n", r);
    double cum_prob = 0.0;
    
    for (int i=0; i<STRATEGY_SIZE; i++) {
        cum_prob += prob_arr[i];
        if (r <= cum_prob) {
            return get_action_by_indx(action_set, i);  // return the action list
        }
            
    }

 }



void applyMotorSpeed_fine_grain(PCA9685* pwm) {
    
    /* set motor spped with fine-grain checking */

    _TEE_VETTED_SET_FREQ(pwm, PWM_FREQUENCY);

    _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL1, pulse_length_esc1);
    _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL2, pulse_length_esc2);
    _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL3, pulse_length_esc3);
    _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL4, pulse_length_esc4);

}


void applyMotorSpeed_intermittent(PCA9685* pwm) {
    
    /* set motor spped with intermittent checking */


    int action_set[N_ACT_CMD] = {0};
    get_rand_command_seq(action_set); 
    enum{_SET_FREQ,_SET_MOTOR_1,_SET_MOTOR_2,_SET_MOTOR_3,_SET_MOTOR_4}; // command names for better readibility

    if (action_set[_SET_FREQ]) {
        _TEE_VETTED_SET_FREQ(pwm, PWM_FREQUENCY);
    }
    else { // vanilla
        pwm->setPWMFreq(PWM_FREQUENCY);
	    usleep(PWM_SLEEP_TIME);
    }

    if (action_set[_SET_MOTOR_1]) {
        _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL1, pulse_length_esc1);
    }
    else { // vanilla
        pwm->setPWM(CHANNEL1, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc1));
        usleep(PWM_SLEEP_TIME);
    }

    if (action_set[_SET_MOTOR_2]) {
        _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL2, pulse_length_esc2);
    }
    else { // vanilla
        pwm->setPWM(CHANNEL2, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc2));
        usleep(PWM_SLEEP_TIME);
    }

    if (action_set[_SET_MOTOR_3]) {
        _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL3, pulse_length_esc3);
    }
    else { // vanilla
        pwm->setPWM(CHANNEL3, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc3));
        usleep(PWM_SLEEP_TIME);
    }

    if (action_set[_SET_MOTOR_4]) {
        _TEE_VETTED_SET_PULSE_LENGTH(pwm, CHANNEL4, pulse_length_esc4);
    }
    else { // vanilla
        pwm->setPWM(CHANNEL4, 0, calcTicks(PWM_FREQUENCY, pulse_length_esc4));
        usleep(PWM_SLEEP_TIME);
    }

}