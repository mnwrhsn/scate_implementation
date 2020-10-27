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

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <plat_rover_ta.h>

enum{SPEED_MIN=80,SPEED_MAX=150};  // speed threshold
enum{FORWARD,BACKWARD,BRAKE,RELEASE};  // motor positions
enum{LEFTMOTOR=3,RIGHTMOTOR=4};  // motor positions
enum{NAV_FORWARD, NAV_LEFT, NAV_RIGHT};  // navigation commands



/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	// DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	// DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	// DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	// IMSG("Plat-rover: Hello World!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	// IMSG("Goodbye!\n");
}



static TEE_Result check_speed(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	// DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	int v = (int) params[0].value.a;  // assign for better readibility

	if (v < SPEED_MIN && v > SPEED_MAX) {
		params[0].value.a = -1;  // error
	}

	return TEE_SUCCESS;
}


static TEE_Result check_nav(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	// DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	
	int motorpos = (int) params[0].value.a;  // assign for better readibility
	int cmd = (int) params[1].value.a;  // assign for better readibility
	int req_cmd = (int) params[1].value.b;  // assign for better readibility

	if (cmd == NAV_FORWARD && req_cmd != FORWARD) {
		params[0].value.a = -1;  // error
		params[1].value.a = -1;  // error
		params[1].value.b = -1;  // error
	}

	if (cmd == NAV_LEFT) {
		if (motorpos == LEFTMOTOR && req_cmd != RELEASE) {
			params[0].value.a = -1;  // error
			params[1].value.a = -1;  // error
			params[1].value.b = -1;  // error
		}
		if (motorpos == RIGHTMOTOR && req_cmd != FORWARD) {
			params[0].value.a = -1;  // error
			params[1].value.a = -1;  // error
			params[1].value.b = -1;  // error
		}
		
	}

	if (cmd == NAV_RIGHT) {
		if (motorpos == LEFTMOTOR && req_cmd != FORWARD) {
			params[0].value.a = -1;  // error
			params[1].value.a = -1;  // error
			params[1].value.b = -1;  // error
		}
		if (motorpos == RIGHTMOTOR && req_cmd != RELEASE) {
			params[0].value.a = -1;  // error
			params[1].value.a = -1;  // error
			params[1].value.b = -1;  // error
		}
		
	}

	return TEE_SUCCESS;
}


/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_PLAT_ROVER_CHECK_SPPED:
		return check_speed(param_types, params);
	case TA_PLAT_ROVER_CHECK_NAV:
		return check_nav(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
