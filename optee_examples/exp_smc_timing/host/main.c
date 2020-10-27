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


/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <exp_smc_timing_ta.h>

#define EXP_FLAG_INIT_CONTEXT 0xF000AAAA
#define EXP_FLAG_OPEN_SESSION 0xF000AAAB
#define EXP_FLAG_INVO_COMMAND 0xF000AAAC
#define EXP_FLAG_CLSE_SESSION 0xF000AAAD
#define EXP_FLAG_FINI_CONTEXT 0xF000AAAE
#define EXP_FLAG_CAL_ENDTOEND 0xF000AAAF


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

long calculate_time_InitializeContext() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	struct timespec vartime = timer_start(); 
	res = TEEC_InitializeContext(NULL, &ctx);
	long time_elapsed_nanos = timer_end(vartime);
	
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return time_elapsed_nanos;

}

long calculate_time_OpenSession() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	 
	res = TEEC_InitializeContext(NULL, &ctx);
	
	
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	struct timespec vartime = timer_start();
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	long time_elapsed_nanos = timer_end(vartime);


	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return time_elapsed_nanos;

}


long calculate_time_InvokeCommand() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	 
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	struct timespec vartime = timer_start();
	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	long time_elapsed_nanos = timer_end(vartime);

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return time_elapsed_nanos;

}

long calculate_time_CloseSession() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	 
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	
	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	struct timespec vartime = timer_start();
	TEEC_CloseSession(&sess);
	long time_elapsed_nanos = timer_end(vartime);

	TEEC_FinalizeContext(&ctx);

	return time_elapsed_nanos;

}


long calculate_time_FinalizeContext() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	 
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	
	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	
	TEEC_CloseSession(&sess);
	
	struct timespec vartime = timer_start();
	TEEC_FinalizeContext(&ctx);
	long time_elapsed_nanos = timer_end(vartime);

	return time_elapsed_nanos;

}

long calculate_time_EndToEnd() 
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMC_TIMING_UUID;
	uint32_t err_origin;
	
	struct timespec vartime = timer_start();
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);

	memset(&op, 0, sizeof(op)); /* Clear the TEEC_Operation struct */
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 200;

	res = TEEC_InvokeCommand(&sess, TA_BOUND_CHECK_VALUE, &op,
				 &err_origin);
	
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	long time_elapsed_nanos = timer_end(vartime);

	return time_elapsed_nanos;

}


void do_calculate_time_by_flag(int count, int __flag) 
{
	// set the filename for writting

	int sleep_duration = 50;  // time in microsec.
	
	char filename[80];
	FILE *fp;

	switch(__flag)
	{
		case EXP_FLAG_INIT_CONTEXT:
			strcpy(filename, "TDATA_INIT_CONTEXT.txt");
			break;

		case EXP_FLAG_OPEN_SESSION:
			strcpy(filename, "TDATA_OPEN_SESSION.txt");
			break;

		case EXP_FLAG_INVO_COMMAND:
			strcpy(filename, "TDATA_INVOKE_COMMAND.txt");
			break;

		case EXP_FLAG_CLSE_SESSION:
			strcpy(filename, "TDATA_CLOSE_SESSION.txt");
			break;

		case EXP_FLAG_FINI_CONTEXT:
			strcpy(filename, "TDATA_FIN_CONTEXT.txt");
			break;
		
		case EXP_FLAG_CAL_ENDTOEND:
			strcpy(filename, "TDATA_END_TO_END.txt");
			break;

		// doesn't match any flag
		default:
			return;
	}

	
	
	long* time_arr = (long*)malloc(count * sizeof(long)); 
  
    // Error checking
    if (time_arr == NULL) { 
        printf("Memory not allocated.\n"); 
        return;
    } 

	for (int i=0; i<count; i++) {

		switch(__flag)
		{
			case EXP_FLAG_INIT_CONTEXT:
				time_arr[i] = calculate_time_InitializeContext();
				break;

			case EXP_FLAG_OPEN_SESSION:
				time_arr[i] = calculate_time_OpenSession();
				break;

			case EXP_FLAG_INVO_COMMAND:
				time_arr[i] = calculate_time_InvokeCommand();
				break;

			case EXP_FLAG_CLSE_SESSION:
				time_arr[i] = calculate_time_CloseSession();
				break;

			case EXP_FLAG_FINI_CONTEXT:
				time_arr[i] = calculate_time_FinalizeContext();
				break;
			
			case EXP_FLAG_CAL_ENDTOEND:
				time_arr[i] = calculate_time_EndToEnd();
				break;

			// doesn't match any flag
			default:
				// time_arr[i] = NULL;
				return;
		}

		usleep(sleep_duration);  // did enough work. take some rest   

		
	}

	// writing data to file

	if((fp=fopen(filename, "w+"))==NULL) {
    	printf("Cannot open file.\n");
  	}

	printf("Writing data to filename: %s ...\n", filename);

	// Print the elements of the array to file 
	// printf("The elements of the timings are:\n"); 
	for (int i=0; i<count; i++) {
		// printf("%ld\n", time_arr[i]); 
		fprintf(fp, "%ld\n", time_arr[i]); 
	} 

	fclose(fp); // close file handler


}


int main(void)
{
	

	int count = 10000;  // number of times each experiment will perform
	int sleep_duration = 500;  // time in microsec.
	
	

	printf("Running experiements for InitializeContext()...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_INIT_CONTEXT);
	usleep(sleep_duration); 
	
	printf("Running experiements for OpenSession()...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_OPEN_SESSION);
	usleep(sleep_duration); 

	printf("Running experiements for InvokeCommand()...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_INVO_COMMAND);
	usleep(sleep_duration); 

	printf("Running experiements for CloseSession()...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_CLSE_SESSION);
	usleep(sleep_duration); 

	printf("Running experiements for FinalizeContext()...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_FINI_CONTEXT);
	usleep(sleep_duration); 

	printf("Running experiements for End-To-End Timings...\n");
	do_calculate_time_by_flag(count, EXP_FLAG_CAL_ENDTOEND);
	usleep(sleep_duration); 


	printf("\n== Experiments Done! Grab file from current directory. Timing data are in nanosec. ==\n");
	return 0;
}
