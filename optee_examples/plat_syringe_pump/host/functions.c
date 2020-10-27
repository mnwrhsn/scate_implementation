
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_syringe_pump_ta.h>

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
            action_set[2] = 1;
            break;
        case 1:
            action_set[0] = 1;
            action_set[1] = 1;
            action_set[3] = 1;
            break;
        case 2:
            action_set[0] = 1;
            action_set[1] = 1;
            action_set[4] = 1;
            break;
        case 3:
            action_set[0] = 1;
            action_set[1] = 1;
            action_set[5] = 1;
            break;
        case 4:
            action_set[0] = 1;
            action_set[1] = 1;
            action_set[6] = 1;
            break;
        case 5:
            action_set[0] = 1;
            action_set[2] = 1;
            action_set[3] = 1;
            break;
        case 6:
            action_set[0] = 1;
            action_set[2] = 1;
            action_set[4] = 1;
            break;
        case 7:
            action_set[0] = 1;
            action_set[2] = 1;
            action_set[5] = 1;
            break;
        case 8:
            action_set[0] = 1;
            action_set[2] = 1;
            action_set[6] = 1;
            break;
        case 9:
            action_set[0] = 1;
            action_set[3] = 1;
            action_set[4] = 1;
            break;
        case 10:
            action_set[0] = 1;
            action_set[3] = 1;
            action_set[5] = 1;
            break;
        case 11:
            action_set[0] = 1;
            action_set[3] = 1;
            action_set[6] = 1;
            break;
        case 12:
            action_set[0] = 1;
            action_set[4] = 1;
            action_set[5] = 1;
            break;
        case 13:
            action_set[0] = 1;
            action_set[4] = 1;
            action_set[6] = 1;
            break;
        case 14:
            action_set[0] = 1;
            action_set[5] = 1;
            action_set[6] = 1;
            break;
        case 15:
            action_set[1] = 1;
            action_set[2] = 1;
            action_set[3] = 1;
            break;
        case 16:
            action_set[1] = 1;
            action_set[2] = 1;
            action_set[4] = 1;
            break;
        case 17:
            action_set[1] = 1;
            action_set[2] = 1;
            action_set[5] = 1;
            break;
        case 18:
            action_set[1] = 1;
            action_set[2] = 1;
            action_set[6] = 1;
            break;
        case 19:
            action_set[1] = 1;
            action_set[3] = 1;
            action_set[4] = 1;
            break;
        case 20:
            action_set[1] = 1;
            action_set[3] = 1;
            action_set[5] = 1;
            break;
        case 21:
            action_set[1] = 1;
            action_set[3] = 1;
            action_set[6] = 1;
            break;
        case 22:
            action_set[1] = 1;
            action_set[4] = 1;
            action_set[5] = 1;
            break;
        case 23:
            action_set[1] = 1;
            action_set[4] = 1;
            action_set[6] = 1;
            break;
        case 24:
            action_set[1] = 1;
            action_set[5] = 1;
            action_set[6] = 1;
            break;
        case 25:
            action_set[2] = 1;
            action_set[3] = 1;
            action_set[4] = 1;
            break;
        case 26:
            action_set[2] = 1;
            action_set[3] = 1;
            action_set[5] = 1;
            break;
        case 27:
            action_set[2] = 1;
            action_set[3] = 1;
            action_set[6] = 1;
            break;
        case 28:
            action_set[2] = 1;
            action_set[4] = 1;
            action_set[5] = 1;
            break;
        case 29:
            action_set[2] = 1;
            action_set[4] = 1;
            action_set[6] = 1;
            break;
        case 30:
            action_set[2] = 1;
            action_set[5] = 1;
            action_set[6] = 1;
            break;
        case 31:
            action_set[3] = 1;
            action_set[4] = 1;
            action_set[5] = 1;
            break;
        case 32:
            action_set[3] = 1;
            action_set[4] = 1;
            action_set[6] = 1;
            break;
        case 33:
            action_set[3] = 1;
            action_set[5] = 1;
            action_set[6] = 1;
            break;
        case 34:
            action_set[4] = 1;
            action_set[5] = 1;
            action_set[6] = 1;
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

/* this is job of a task */
void bolus_vanilla(Adafruit_StepperMotor* stepper) {

    // printf("Got stepper\n");
	stepper->setSpeed(MOTOR_SPEED_RPM); //  set speed
    usleep(2*SLEEP_TIME);
	// printf("Steps: %d\n", steps);

    // PUSH events
    // printf("Push events...\n");
    for (int i=0; i<steps; i++) {
        stepper->step(1, FORWARD,  SINGLE);
        usleep(SLEEP_TIME);
    }

    usleep(2*SLEEP_TIME);

    // PULL events
    // printf("Pull events...\n");
    for (int i=0; i<steps; i++) {
        stepper->step(1, BACKWARD,  SINGLE);
        usleep(SLEEP_TIME);
    }

    // printf("Command id _SET_SPEED: %d\n", _SET_SPEED);

}


void _TEE_VETTED_SET_SPPED(Adafruit_StepperMotor* stepper, int speed) {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	uint32_t err_origin;

    // printf("Called TEE vetted SET_SPEED...\n");
	
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
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
	// printf("Invoking TA with val %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_SYRINGE_PUMP_CHECK_SPPED, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
    int _teeout = op.params[0].value.a;
	// printf("TA returned %d\n", op.params[0].value.a);
	TEEC_CloseSession(&sess);	
	TEEC_FinalizeContext(&ctx);

    if (_teeout < 0) {
        // TEE said invalid --> verification failed. Print an error
        printf("Set speed verification failed\n");
    }
    else {
        // set speed
        // printf("Setting step from TEE func...\n");
        stepper->setSpeed(speed); //  set speed
    }
    
}


void _TEE_VETTED_PUSH_PULL(Adafruit_StepperMotor* stepper, int event, int direction) {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	uint32_t err_origin;
    
    // printf("Called TEE vetted PUSH/PULL...\n");
	
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
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
	op.params[0].value.a = event;
    op.params[0].value.b = direction;
	// printf("Invoking TA with val %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_SYRINGE_PUMP_CHECK_PUSH_PULL, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
    int _teeout[2] = {0};
    _teeout[0] = op.params[0].value.a;
    _teeout[1] = op.params[0].value.b;
	// printf("TA returned: event%d, direction%d\n", _teeout[0], _teeout[1]);
	TEEC_CloseSession(&sess);	
	TEEC_FinalizeContext(&ctx);

    if (_teeout[0] < 0 || _teeout[0] < 0) {
        // TEE verification failed. Print an error
        printf("PUSH/PULL verification failed\n");
    }
    else {
        // set push/pull event
        // printf("Setting PUSH/PULL from TEE func...\n");
        if (event==PUSH)
            stepper->step(1, FORWARD,  SINGLE);
        else if (event==PULL)
            stepper->step(1, BACKWARD,  SINGLE);
    }
    
}

/* this is job of a task [for fine grain] */
void bolus_fine_grain(Adafruit_StepperMotor* stepper) {

    // printf("Got stepper [Fine Grain]\n");
	stepper->setSpeed(MOTOR_SPEED_RPM); //  set speed

    _TEE_VETTED_SET_SPPED(stepper, MOTOR_SPEED_RPM);

    usleep(2*SLEEP_TIME);
	// printf("Steps: %d\n", steps);

    // PUSH events
    // printf("Push events...\n");
    for (int i=0; i<steps; i++) {
        // stepper->step(1, FORWARD,  SINGLE);
        _TEE_VETTED_PUSH_PULL(stepper, PUSH, FORWARD);
        usleep(SLEEP_TIME);
    }

    usleep(2*SLEEP_TIME);

    // PULL events
    // printf("Pull events...\n");
    for (int i=0; i<steps; i++) {
        // stepper->step(1, BACKWARD,  SINGLE);
         _TEE_VETTED_PUSH_PULL(stepper, PULL, BACKWARD);
        usleep(SLEEP_TIME);
    }

    // printf("Command id _SET_SPEED: %d\n", _SET_SPEED);

}



/* this is job of a task [for intermittent checking] */
void bolus_intermittent(Adafruit_StepperMotor* stepper) {

    int action_set[N_ACT_CMD] = {0};
    get_rand_command_seq(action_set); 

   
    // set speed
    if (action_set[_SET_SPEED])
        _TEE_VETTED_SET_SPPED(stepper, MOTOR_SPEED_RPM); // TEE vetted set speed
    else
        stepper->setSpeed(MOTOR_SPEED_RPM); //  vanilla set speed

    usleep(2*SLEEP_TIME);
	// printf("Steps: %d\n", steps);

    // PUSH events
    // printf("Push events...\n");
    for (int i=1; i<=steps; i++) {       
        if (action_set[i])
          _TEE_VETTED_PUSH_PULL(stepper, PUSH, FORWARD); // TEE vetted PUSH
        else
            stepper->step(1, FORWARD,  SINGLE); //  vanilla PUSH

        usleep(SLEEP_TIME);
    }

    usleep(2*SLEEP_TIME);

    // PULL events
    // printf("Pull events...\n");
    for (int i=steps+1; i<=2*steps; i++) {
        if (action_set[i])
           _TEE_VETTED_PUSH_PULL(stepper, PULL, BACKWARD); // TEE vetted PULL
        else
            stepper->step(1, BACKWARD,  SINGLE); //  vanilla PULL

        usleep(SLEEP_TIME);
    }

    // printf("Command id _SET_SPEED: %d\n", _SET_SPEED);

}