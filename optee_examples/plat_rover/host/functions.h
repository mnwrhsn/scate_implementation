
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_rover_ta.h>

/* include MotorHat header */
#include "Adafruit_MotorHAT.h"

// TEE UUID
const TEEC_UUID uuid = TA_PLAT_ROVER_UUID;


/* -- Enums and constants -- */
enum{LEFTMOTOR=3,RIGHTMOTOR=4};  // motor positions
enum{NAV_FORWARD, NAV_LEFT, NAV_RIGHT};  // navigation commands




/* -- Constants -- */

#define MOTOR_DELAY 1000*100 // motor sleep time
#define SLEEP_TIME 1000*100  // microsecond

#define NUM_LINE_SENSOR 5
const int white[NUM_LINE_SENSOR]={767,815,859,710,700}; //reading when all sensors are at white
const int black[NUM_LINE_SENSOR]={1012,1013,1015,1003,1004}; //#reading when all sensors are black
// const int range_col[NUM_LINE_SENSOR] = {0}; //difference between black and white
const int multp[NUM_LINE_SENSOR]={-100,-50,0,50,100}; //Add a multipler to each sensor

const int SPEED_LEFT = 80;
const int SPEED_RIGHT = 80;
const int SPEED_FORWARD = 125;

/* some params for intermittent checking */

// Command index: 
// ---------------

// _SET_SPEED_LEFT		0  // set left motor speed 
// _SET_NAV_LEFT 		1  // set left nav
// _SET_SPPED_RIGHT	    2  // set right motor speed
// _SET_NAV_RIGHT		3  // set right nav


// Output from game:
// ------------------
// Rover Strategy: [(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (2, 3)]
// Rover Prob: [0.1, 0.4986981117669036, 0.1, 0.1, 0.10130188823309631, 0.1]
// Size of Rover Strategy set: 6

enum{_SET_SPEED_LEFT, _SET_NAV_LEFT, _SET_SPPED_RIGHT, _SET_NAV_RIGHT}; // command ids for intermittent checking
#define N_ACT_CMD    4     // number of actuation commands 4 (index 0, 4)
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

// some functions for intermittent checking
void get_action_by_indx(int* action_set, int indx);
void get_rand_command_seq(int* action_set);

// functions definitions
void motor_job_vanilla();
void motor_job_fine_grain();
void motor_job_intermittent();

#endif