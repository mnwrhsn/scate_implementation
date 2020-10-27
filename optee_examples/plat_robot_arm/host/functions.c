
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_robot_arm_ta.h>


/* include PCA header */
#include "PCA9685.h"

/* include helper functions */
#include "functions.h"

/*  C reimplementation of robot arm
    source: https://github.com/hemstreet/RobotArm-6DOF
 */




int move_servo(PCA9685 pwm, int channel, int amount) {
    if (amount > servo_max || amount < servo_min) {
        printf("Out of servo range\n");
        return -1;
    }

    pwm.setPWM(channel, 0, amount);
    return 0;
            
}

 int move_servo_to_percent(PCA9685 pwm, int channel, int percent) {

    int value = int(((percent / 100.0) * (servo_max - servo_min)) + servo_min);
    return move_servo(pwm, channel, value);

 }

 void pick_drop_object(PCA9685 pwm, int percent) {

    for (int i=0; i<GRIP_MOVE; i++)
        move_servo_to_percent(pwm, wrist_flex, percent);

 }

 void move_arm(PCA9685 pwm, int percent) {

    for (int i=0; i<ARM_MOVE; i++)
        move_servo_to_percent(pwm, hand, percent);

 }


void __TEE_vetted_actuation(PCA9685 pwm, int percent, int command) {

    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_ROBOT_ARM_UUID;
	uint32_t err_origin;

    int TEE_COMMAND;

    switch(command)
    {
        case TEE_CMD_MOVE_GRIP:
            TEE_COMMAND = TA_PLAT_ROBOT_ARM_CMD_CHECK_GRIP;
            break;

        case TEE_CMD_MOVE_HAND:
            TEE_COMMAND = TA_PLAT_ROBOT_ARM_CMD_CHECK_HAND;
            break;

        // does not match any command
        default:
            return;
    }


	
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
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
	op.params[0].value.a = percent;
	
	
	// printf("Invoking TA to check %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TEE_COMMAND, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);


    // TEE verifies
    if ((int) op.params[0].value.a ==  percent) {
        switch(command)
        {
            case TEE_CMD_MOVE_GRIP:
                pick_drop_object(pwm, percent);
                break;

            case TEE_CMD_MOVE_HAND:
                move_arm(pwm, percent);
                break;

            // does not match any command
            default:
                break;  // do nothing
        }
    }
    else
        printf("Value mismatch! Servo not called.\n");    // informing about anomaly

	// printf("TA returned the value: %d\n", op.params[0].value.a);
	
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);


 }

/* returns the random command index for checking based on game */
// idea from:https://stackoverflow.com/questions/9330394/how-to-pick-an-item-by-its-probability
// called Fitness proportionate selection, also known as roulette wheel selection
// Index:          0      1        2      3       4       5
// Action set: [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)]
// Command index:
//       _PICK   0  // pick object
//       _MOVE   1  // move hand
//       _DROP   2  // drop object
//       _RESET  3  // reset hand
void get_action_by_indx(int* action_set, int indx) { 

    // printf("Indx is %d\n", indx);
   
    switch(indx) {
        case 0:
            action_set[_PICK] = 1;
            action_set[_MOVE] = 1;
            break;
        case 1:
            action_set[_PICK] = 1;
            action_set[_DROP] = 1;
            break;
        case 2:
            action_set[_PICK] = 1;
            action_set[_RESET] = 1;
            break;
        case 3:
            action_set[_MOVE] = 1;
            action_set[_DROP] = 1;
            break;
        case 4:
            action_set[_MOVE] = 1;
            action_set[_RESET] = 1;
            break;
        case 5:
            action_set[_DROP] = 1;
            action_set[_RESET] = 1;
            break;
        default:
            // do nothing
            printf("Not matching any index. Returning...\n");
        
   }

} 

void get_rand_command_seq(int* action_set) {

    double r = (double)rand() / (double)((unsigned)RAND_MAX + 1);
    double cum_prob = 0.0;
    // int* action_set;

    for (int i=0; i<STRATEGY_SIZE; i++) {
        cum_prob += prob_arr[i];
        if (r <= cum_prob) {
            return get_action_by_indx(action_set, i);  // return the action list
        }
            
    }

 }
