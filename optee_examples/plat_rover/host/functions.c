
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_rover_ta.h>

/* include helper functions */
#include "functions.h"

/* include MotorHat header */
#include "Adafruit_MotorHAT.h"

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
            action_set[1] = 1;
            action_set[2] = 1;
            break;
        case 4:
            action_set[1] = 1;
            action_set[3] = 1;
            break;
        case 5:
            action_set[2] = 1;
            action_set[3] = 1;
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

// simulate sensor reading
// checkout GOPIOG sensor reading codes 
// https://github.com/mnwrhsn/rt_io_sec_userspace/blob/master/gpg_rov_exp/dos_exp/gopigo.c
int get_postion()
{   
    unsigned char buffer[60] = {0};
    int read_val[NUM_LINE_SENSOR] = {0};
    
    int flag = 0;
    int i;
    int curr_pos=0;
    int percent_black_line[NUM_LINE_SENSOR]={0};
    int range_col[NUM_LINE_SENSOR] = {0}; //difference between black and white
    int diff_val[NUM_LINE_SENSOR];

    do_i2c_write(buffer, 5);   // send write command for reading sensors
    usleep(MOTOR_DELAY);  // wait a bit
    do_i2c_read(buffer, 10);   // read sensor readings
    usleep(MOTOR_DELAY);  // wait a bit

    for (i=0;i<2*NUM_LINE_SENSOR;i++) {
        buffer[i] = rand() % 1024;  // random number in range 
        // printf("i: %d, buffer %d\n", i, buffer[i]);
    }
        

    for(i=0;i<2*NUM_LINE_SENSOR;i=i+2) {                    // To convert the 10 bit analog reading of each sensor to decimal and store it in read_val[]
		read_val[i/2]=buffer[i]*256+buffer[i+1]; // Values less than 100 - White, Values greater than 800- Black
        if (read_val[i/2] > 65000)              // Checking for junk values in the input
			flag=1;
	}
    if (flag==1) {
        for(i=0;i<NUM_LINE_SENSOR;i++)
            read_val[i]=-1;                    // Making junk input values to -1
    }

    /* idea from: https://github.com/DexterInd/DI_Sensors/blob/master/Python/di_sensors/red_line_follower/line_follower/line_follow.py */

    for(i=0;i<NUM_LINE_SENSOR;i++) {
        range_col[i] = black[i] - white[i];
        diff_val[i] = read_val[i] - white[i];
        percent_black_line[i]=diff_val[i]*100/range_col[i];
        curr_pos+=percent_black_line[i]*multp[i];
    }

    // a busy loop
    for (unsigned j = 0; j < 10000; j++) {
        __asm__ __volatile__ ("" : "+g" (j) : :);
    }

    return curr_pos;
    

}


void init(Adafruit_MotorHAT* hat) {

    Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
    Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port
    
    // start with off
    leftmotor.run(RELEASE);
    rightmotor.run(RELEASE);

}

void release(Adafruit_MotorHAT* hat) {

    Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
    Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port
    
    // reset motors
    leftmotor.run(RELEASE);
    rightmotor.run(RELEASE);

}

// void forward(Adafruit_MotorHAT* hat, int speed) {

//     Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
//     Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

//     leftmotor.setSpeed(speed);
//     leftmotor.run(FORWARD);

//     rightmotor.setSpeed(speed);
//     rightmotor.run(FORWARD);
// }

// void left(Adafruit_MotorHAT* hat, int speed) {

//     Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
//     Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

//     leftmotor.setSpeed(0);
//     leftmotor.run(RELEASE);

//     rightmotor.setSpeed(speed);
//     rightmotor.run(FORWARD);
// }

// void right(Adafruit_MotorHAT* hat, int speed) {

//     Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
//     Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

//     leftmotor.setSpeed(speed);
//     leftmotor.run(FORWARD);

//     rightmotor.setSpeed(0);
//     rightmotor.run(RELEASE);
// }


// void __TEE_VETTED_forward(Adafruit_MotorHAT* hat, int speed, int __left_cmd, int __right_cmd) {

//     Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
//     Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

//     leftmotor.setSpeed(speed);
//     leftmotor.run(FORWARD);

//     rightmotor.setSpeed(speed);
//     rightmotor.run(FORWARD);
// }


void __TEE_VETTED_SET_SPEED(Adafruit_MotorHAT* hat, int motorpos, int speed) {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	uint32_t err_origin;
	
	res = TEEC_InitializeContext(NULL, &ctx); /* Initialize a context connecting us to the TEE */
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
	
	
	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = speed;
	
	// printf("Invoking TA with argument %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_PLAT_ROVER_CHECK_SPPED, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	// printf("TA returned %d\n", op.params[0].value.a);

    int _teeout= (int) op.params[0].value.a;  // save value returned by TEE

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

    // TEE verified OK
    if (_teeout >=0) {
        Adafruit_DCMotor& motor = hat->getDC(motorpos);    // get motor port
        motor.setSpeed(speed);
    }
    else {
        printf("TEE Verification failed while setting motor spped!\n"); // print an error message
    }
	
}


void __TEE_VETTED_SET_NAV(Adafruit_MotorHAT* hat, int motorpos, int nav_action, Direction requested_action) {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	uint32_t err_origin;
	
	res = TEEC_InitializeContext(NULL, &ctx); /* Initialize a context connecting us to the TEE */
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
	
	
	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INOUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = motorpos;
    op.params[1].value.a = nav_action;
    op.params[1].value.b = (int) requested_action;

	
	// printf("Invoking TA with argument %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_PLAT_ROVER_CHECK_NAV, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	
    // printf("TA returned %d\n", op.params[0].value.a);
    // printf("TA returned %d\n", op.params[1].value.a);
    // printf("TA returned %d\n", op.params[1].value.b);

    int _teeout[3] = {0};
    _teeout[0]= (int) op.params[0].value.a;  // save value returned by TEE
    _teeout[1]= (int) op.params[1].value.a;  // save value returned by TEE
    _teeout[2]= (int) op.params[1].value.b;  // save value returned by TEE

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

    // TEE verified OK
    if (_teeout[0] >=0 && _teeout[1] >=0 && _teeout[2] >=0) {
        Adafruit_DCMotor& motor = hat->getDC(motorpos);    // get motor port
        motor.run(requested_action);
    }
    else {
        printf("TEE Verification failed while setting motor spped!\n"); // print an error message
    }
	
}

void do_navigation_vanilla(Adafruit_MotorHAT* hat, int cmd, int speed) {

    Direction leftmotor_cmd;
    Direction rightmotor_cmd;
    int leftmotor_speed;
    int rightmotor_speed;
    Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
    Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

    switch(cmd) {
        case NAV_FORWARD:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = speed;
            rightmotor_speed = speed;
            break;
        case NAV_LEFT:
            leftmotor_cmd = RELEASE;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = 0;
            rightmotor_speed = speed;
            break;
        case NAV_RIGHT:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = RELEASE;
            leftmotor_speed = speed;
            rightmotor_speed = 0;
            break;
        default:
            // do nothing
            printf("Not matching any index. Returning...\n");
            return;
    }

    leftmotor.setSpeed(leftmotor_speed);
    leftmotor.run(leftmotor_cmd);

    rightmotor.setSpeed(rightmotor_speed);
    rightmotor.run(rightmotor_cmd);
}

void do_navigation_fine_grain(Adafruit_MotorHAT* hat, int cmd, int speed) {

    Direction leftmotor_cmd;
    Direction rightmotor_cmd;
    int leftmotor_speed;
    int rightmotor_speed;
    // Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
    // Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

    switch(cmd) {
        case NAV_FORWARD:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = speed;
            rightmotor_speed = speed;
            break;
        case NAV_LEFT:
            leftmotor_cmd = RELEASE;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = 0;
            rightmotor_speed = speed;
            break;
        case NAV_RIGHT:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = RELEASE;
            leftmotor_speed = speed;
            rightmotor_speed = 0;
            break;
        default:
            // do nothing
            printf("Not matching any index. Returning...\n");
            return;
    }

    __TEE_VETTED_SET_SPEED(hat, LEFTMOTOR, leftmotor_speed);
    __TEE_VETTED_SET_NAV(hat, LEFTMOTOR, cmd, leftmotor_cmd);
    __TEE_VETTED_SET_SPEED(hat, RIGHTMOTOR, rightmotor_speed);
    __TEE_VETTED_SET_NAV(hat, RIGHTMOTOR, cmd, rightmotor_cmd);

    // leftmotor.setSpeed(leftmotor_speed);
    // leftmotor.run(leftmotor_cmd);
    // rightmotor.setSpeed(rightmotor_speed);
    // rightmotor.run(rightmotor_cmd);
}


void do_navigation_intermittent(Adafruit_MotorHAT* hat, int cmd, int speed) {

    Direction leftmotor_cmd;
    Direction rightmotor_cmd;
    int leftmotor_speed;
    int rightmotor_speed;
    Adafruit_DCMotor& leftmotor = hat->getDC(LEFTMOTOR);    // get motor port
    Adafruit_DCMotor& rightmotor = hat->getDC(RIGHTMOTOR);   // get motor port

    int action_set[N_ACT_CMD] = {0};
	get_rand_command_seq(action_set); 

    switch(cmd) {
        case NAV_FORWARD:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = speed;
            rightmotor_speed = speed;
            break;
        case NAV_LEFT:
            leftmotor_cmd = RELEASE;
            rightmotor_cmd = FORWARD;
            leftmotor_speed = 0;
            rightmotor_speed = speed;
            break;
        case NAV_RIGHT:
            leftmotor_cmd = FORWARD;
            rightmotor_cmd = RELEASE;
            leftmotor_speed = speed;
            rightmotor_speed = 0;
            break;
        default:
            // do nothing
            printf("Not matching any index. Returning...\n");
            return;
    }

    // __TEE_VETTED_SET_SPEED(hat, LEFTMOTOR, leftmotor_speed);
    // __TEE_VETTED_SET_NAV(hat, LEFTMOTOR, cmd, leftmotor_cmd);
    // __TEE_VETTED_SET_SPEED(hat, RIGHTMOTOR, rightmotor_speed);
    // __TEE_VETTED_SET_NAV(hat, RIGHTMOTOR, cmd, rightmotor_cmd);

    // leftmotor.setSpeed(leftmotor_speed);
    // leftmotor.run(leftmotor_cmd);
    // rightmotor.setSpeed(rightmotor_speed);
    // rightmotor.run(rightmotor_cmd);

    if (action_set[_SET_SPEED_LEFT])
        __TEE_VETTED_SET_SPEED(hat, LEFTMOTOR, leftmotor_speed);
    else
        leftmotor.setSpeed(leftmotor_speed);

    if (action_set[_SET_NAV_LEFT])
        __TEE_VETTED_SET_NAV(hat, LEFTMOTOR, cmd, leftmotor_cmd);
    else
        leftmotor.run(leftmotor_cmd);

    if (action_set[_SET_SPPED_RIGHT])
        __TEE_VETTED_SET_SPEED(hat, RIGHTMOTOR, rightmotor_speed);
    else
        rightmotor.setSpeed(rightmotor_speed);

     if (action_set[_SET_NAV_RIGHT])
        __TEE_VETTED_SET_NAV(hat, RIGHTMOTOR, cmd, rightmotor_cmd);
    else
        rightmotor.run(rightmotor_cmd);

}


void motor_job_vanilla() {
    /* hat initialization */
	Adafruit_MotorHAT hat;
    init(&hat);

    int curr_pos = get_postion();

    // printf("Current position: %d\n", curr_pos);

    if (curr_pos <-2500) {
        do_navigation_vanilla(&hat, NAV_RIGHT, SPEED_RIGHT);
    }

	else if (curr_pos >2500) {
        do_navigation_vanilla(&hat, NAV_LEFT, SPEED_LEFT);
    }
    else {
       do_navigation_vanilla(&hat, NAV_FORWARD, SPEED_FORWARD);
    }
   
    usleep(SLEEP_TIME);
    release(&hat);
    usleep(SLEEP_TIME);
    
}


void motor_job_fine_grain() {
    /* hat initialization */
	Adafruit_MotorHAT hat;
    init(&hat);

    int curr_pos = get_postion();

    // printf("Current position: %d\n", curr_pos);

    if (curr_pos <-2500) {
        do_navigation_fine_grain(&hat, NAV_RIGHT, SPEED_RIGHT);
    }

	else if (curr_pos >2500) {
        do_navigation_fine_grain(&hat, NAV_LEFT, SPEED_LEFT);
    }
    else {
       do_navigation_fine_grain(&hat, NAV_FORWARD, SPEED_FORWARD);
    }
   
    usleep(SLEEP_TIME);
    release(&hat);
    usleep(SLEEP_TIME);
    
}



void motor_job_intermittent() {
    /* hat initialization */
	Adafruit_MotorHAT hat;
    init(&hat);

    int curr_pos = get_postion();

    // printf("Current position: %d\n", curr_pos);

    if (curr_pos <-2500) {
        do_navigation_intermittent(&hat, NAV_RIGHT, SPEED_RIGHT);
    }

	else if (curr_pos >2500) {
        do_navigation_intermittent(&hat, NAV_LEFT, SPEED_LEFT);
    }
    else {
       do_navigation_intermittent(&hat, NAV_FORWARD, SPEED_FORWARD);
    }
   
    usleep(SLEEP_TIME);
    release(&hat);
    usleep(SLEEP_TIME);
    
}
