#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_flight_controller_ta.h>


/* include PCA header */
#include "PCA9685.h"

// #define PWM_FREQUENCY 50  // servo frequency 50 Hz

const TEEC_UUID uuid = TA_FLIGHT_CONTROLLER_UUID; // set TA UUID

#define SLEEP_TIME 1000*100  // microsecond

/* Loops and attacks */
#define NUM_OF_EXP_TRIAL 1000
#define MAX_JOBS 1000

// for timing experiments
#define TIMING_EXP_ITER 1000  // number of counts
#define EXP_VANILLA 200 // some naming for better readibility
#define EXP_FINE_GRAIN 300
#define EXP_IC 400
#define _ATTACK_CMD_ID 2 // motor 2


/* function defines */
void flc_job_vanilla();
void flc_job_fine_grain();
void flc_job_intermittent();

// some functions for intermittent checking
extern void get_rand_command_seq(int* action_set);
extern const int N_ACT_CMD;



#endif
