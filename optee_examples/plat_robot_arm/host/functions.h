

#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_robot_arm_ta.h>


/* include PCA header */
#include "PCA9685.h"

const int servo_min = 150;  // Min pulse length out of 4096
const int servo_max = 600;  // Max pulse length out of 4096

#define FREQUENCY 50  // servo frequency 50 Hz

#define SLEEP_TIME 1000*100  // microsecond

// robot hands [servo channels]
const int hand = 5;
// const int wrist_rotate = 4;
const int wrist_flex = 1;


const int grip_angle = 20;  // angle to move grips
const int hand_angle = 40; // angle to move hand


// iteration count
const int ARM_MOVE = 100;
const int GRIP_MOVE = 50;

// some defines for better readibility
#define TEE_CMD_MOVE_GRIP 0
#define TEE_CMD_MOVE_HAND 1


/* 
   Selection probabilites.
   We have 4 commands (N=4)
   Select 2 commands each job (K=2)
   
   Command index:
      _PICK   0  // pick object
      _MOVE   1  // move hand
      _DROP   2  // drop object
      _RESET  3  // reset hand

   Action set: [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)]
   Probabilities: [0.1, 0.498, 0.1, 0.1, 0.101, 0.1]
 */


#define N_ACT_CMD    4     // number of actuation commands 4 (index 0, 3)
#define _PICK        0       // pick object (index)
#define _MOVE        1       // move hand (index)
#define _DROP        2       // drop object (index)
#define _RESET       3       // reset hand (index)

#define STRATEGY_SIZE 6    // size of the strategy set
const double prob_arr[STRATEGY_SIZE] = {0.1, 0.498, 0.1, 0.1, 0.101, 0.1};

/* Loops and attacks */
#define NUM_OF_EXP_TRIAL 1000
#define MAX_JOBS 1000


// for timing experiments
#define TIMING_EXP_ITER 1000  // number of counts
#define EXP_VANILLA 200 // some naming for better readibility
#define EXP_FINE_GRAIN 300
#define EXP_IC 400


/* function prototypes */

int move_servo(PCA9685 pwm, int channel, int amount);

/* 0-50 clockwise 
   50-100 anticlockwise
   say clockwise 20% --> use 20
   anticlockwise 20% --> use 100-20 = 80
*/
int move_servo_to_percent(PCA9685 pwm, int channel, int percent);

// pick or drop the object
void pick_drop_object(PCA9685 pwm, int percent);

// move arm
void move_arm(PCA9685 pwm, int percent);


// pick/drop (vetted by TEE)
void __TEE_vetted_actuation(PCA9685 pwm, int percent, int command);

// get action list by index
void get_action_by_indx(int* action_set, int indx);

// get random command sequence to check
void get_rand_command_seq(int* action_set);

#endif


