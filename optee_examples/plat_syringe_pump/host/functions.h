
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_syringe_pump_ta.h>

/* include MotorHat header */
#include "Adafruit_MotorHAT.h"

// TEE UUID
const TEEC_UUID uuid = TA_PLAT_SYRINGE_PUMP_UUID;


/* -- Enums and constants -- */

/* -- Constants -- */
#define MOTOR_SPEED_RPM 50  // motor speed
const int steps = 3;  // step to keep motor running

#define SLEEP_TIME 1000*100  // microsecond


// Command index: 
// ---------------
// _SET_SPEED 0
// _PUSH_1 1
// _PUSH_2 2
// _PUSH_3 1
// _PULL_1 4
// _PULL_2 5
// _PULL_3 6

// Action set:
// [(0, 1, 2), (0, 1, 3), (0, 1, 4), (0, 1, 5), (0, 1, 6), 
// (0, 2, 3), (0, 2, 4), (0, 2, 5), (0, 2, 6), (0, 3, 4), 
// (0, 3, 5), (0, 3, 6), (0, 4, 5), (0, 4, 6), (0, 5, 6), 
// (1, 2, 3), (1, 2, 4), (1, 2, 5), (1, 2, 6), (1, 3, 4), 
// (1, 3, 5), (1, 3, 6), (1, 4, 5), (1, 4, 6), (1, 5, 6), 
// (2, 3, 4), (2, 3, 5), (2, 3, 6), (2, 4, 5), (2, 4, 6), 
// (2, 5, 6), (3, 4, 5), (3, 4, 6), (3, 5, 6), (4, 5, 6)]

// Comand IDS
#define N_ACT_CMD    7     // number of actuation commands 7 (index 0, 6)
enum{_SET_SPEED,_PUSH_1,_PUSH_2,_PUSH_3,_PULL_1,_PULL_2,_PULL_3};
enum{PUSH, PULL}; // stepper events

#define STRATEGY_SIZE 35    // size of the strategy set
const double prob_arr[STRATEGY_SIZE] = {    0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 0.02857142857142857, 
                                            0.02857142857142857, 0.02857142857142857, 0.02857142857142857
                                        };

// ATTACK job is _PULL_1 [index 4] (toggle first PULL to PUSH so that wrong amount of fluid is injected)

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

void bolus_vanilla(Adafruit_StepperMotor* stepper);
void bolus_fine_grain(Adafruit_StepperMotor* stepper);
void bolus_intermittent(Adafruit_StepperMotor* stepper);

#endif