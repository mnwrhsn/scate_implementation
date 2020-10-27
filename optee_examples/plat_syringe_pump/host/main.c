/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <plat_syringe_pump_ta.h>

/* include MotorHat header */
#include "Adafruit_MotorHAT.h"

/* include our functions */
#include "functions.h"

static Adafruit_MotorHAT* gHat; // void pointer for signal handler

// source: https://stackoverflow.com/questions/6749621/how-to-create-a-high-resolution-timer-in-linux-to-measure-program-performance

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
	// clock_gettime(CLOCK_MONOTONIC, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
	// clock_gettime(CLOCK_MONOTONIC, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}


/* run experiments to see how many jobs takes to detect the attack */
void run_ic_detection_time_exp() {

	char filename[80];
	strcpy(filename, "ATTACK_DETECT_JOB_COUNT_SYRINGE_PUMP_IC.txt");
	FILE *fp;

	printf("Running %d times to detect number of jobs:\n", NUM_OF_EXP_TRIAL);


	int count_arr[NUM_OF_EXP_TRIAL] = {-1};  // intialize
	for (int i=0; i<NUM_OF_EXP_TRIAL; i++) {
		for (int jc=1; jc<=MAX_JOBS; jc++) {
			int action_set[N_ACT_CMD] = {0};
			get_rand_command_seq(action_set); 
			if (action_set[_PULL_1]) {		// we attack RESET command (synchonization attack)
				count_arr[i] = jc;			// attack detected
				break;
			}
		}
	}

	// writing data to file

	if((fp=fopen(filename, "w+"))==NULL) {
    	printf("Cannot open file.\n");
  	}

	printf("Writing data to filename: %s ...\n", filename);

	// Print the elements of the array to file 
	for (int i=0; i<NUM_OF_EXP_TRIAL; i++) {
		// printf("Trial %d, Job count %d\n", i, count_arr[i]);
		fprintf(fp, "%d\n", count_arr[i]); 
	} 

	fclose(fp); // close file handler


}

long get_time_by_exp_type(Adafruit_StepperMotor* stepper, int exp_type) {

	struct timespec vartime;

	if (exp_type==EXP_VANILLA) {
		vartime = timer_start();
		bolus_vanilla(stepper);
		return timer_end(vartime);

	}
	else if (exp_type==EXP_FINE_GRAIN) {
		vartime = timer_start();
		bolus_fine_grain(stepper);
		return timer_end(vartime);
	}
	else if (exp_type==EXP_IC) {
		vartime = timer_start();
		bolus_intermittent(stepper);
		return timer_end(vartime);

	}
	else {
		printf("Invalid Exp name\n" );
		return NULL;
	}

}

/* run timing experiment */
void run_timing_experiment_by_type(int exp_type) {

	Adafruit_MotorHAT hat;
	char filename[80];
	
	switch(exp_type)
	{
		case EXP_VANILLA:
			strcpy(filename, "TDATA_SYRINGE_PUMP_VANILLA.txt");
			break;
		case EXP_FINE_GRAIN:
			strcpy(filename, "TDATA_SYRINGE_PUMP_FG.txt");
			break;
		case EXP_IC:
			strcpy(filename, "TDATA_SYRINGE_PUMP_IC.txt");
			break;
		// doesn't match any experiment type
		default:
			return;
	}

	FILE *fp;
	long* time_arr = (long*)malloc(TIMING_EXP_ITER * sizeof(long)); 

	/* hat initialization */
	auto& stepper = hat.getStepper(1); // # motor port #1
	
	// Error checking
    if (time_arr == NULL) { 
        printf("Memory not allocated for timing.\n"); 
        return;
    } 

	for (int i=0; i<TIMING_EXP_ITER; i++) {

		printf("Timing Experiment -> File: %s :: Trial # %d\n", filename, i);
		time_arr[i] = get_time_by_exp_type(&stepper, exp_type); // get and save time
		usleep(3*SLEEP_TIME);
	}

	// close hat
	hat.resetAll();

	// writing data to file

	if((fp=fopen(filename, "w+"))==NULL) {
    	printf("Cannot open file.\n");
  	}

	printf("Writing data to filename: %s ...\n", filename);

	// Print the elements of the array to file 
	for (int i=0; i<TIMING_EXP_ITER; i++) {
		// printf("Trial %d, Time: %ld\n", i, time_arr[i]);
		fprintf(fp, "%ld\n", time_arr[i]); 
	} 

	fclose(fp); // close file handler

}

void timeout_handler(int s) {
	printf("Caught timeout signal %d. Resetting hat..\n");
	gHat->resetAll();
	exit(1); 
}

void run_load_memory_exp(int exp_type) {

	/* hat initialization */
	Adafruit_MotorHAT hat;
	auto& stepper = hat.getStepper(1); // # motor port #1
	gHat = &hat; // update to global address

	signal(SIGTERM, timeout_handler);  // reset hat if timeout occurs

	usleep(2*SLEEP_TIME);
	
	for (;;) {
	
		if (exp_type==EXP_VANILLA)
			bolus_vanilla(&stepper);
		else if (exp_type==EXP_FINE_GRAIN)
			bolus_fine_grain(&stepper);
		else if (exp_type==EXP_IC)
			bolus_intermittent(&stepper);

		usleep(2*SLEEP_TIME);
	
	}
}


int main(void)
{
	
	// srand(time(0));  // different seed for each run
	
	/* run time to detect attacks (number of jobs) by Intermittent Checking scheme */
	// run_ic_detection_time_exp();
	// usleep(10*SLEEP_TIME);

	/* run timing experiment for all three schemes */
	// run_timing_experiment_by_type(EXP_VANILLA);
	// usleep(10*SLEEP_TIME);
	// run_timing_experiment_by_type(EXP_FINE_GRAIN);
	// usleep(10*SLEEP_TIME);
	// run_timing_experiment_by_type(EXP_IC);
	// usleep(10*SLEEP_TIME);

	/* 
	*	run load experiments 
	*	NOTE: only one function can be compiled at a time. go to cpuload_exp directory in top folder to see how to run this 
	*/
	
	// run_load_memory_exp(EXP_VANILLA);
	// run_load_memory_exp(EXP_FINE_GRAIN);
	// run_load_memory_exp(EXP_IC);



	printf("Experiment finished!\n");



	return 0;
}
