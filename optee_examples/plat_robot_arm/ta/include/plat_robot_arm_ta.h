/*
 * Copyright (c) 2016-2017, Linaro Limited
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
#ifndef TA_PLAT_ROBOT_ARM_H
#define TA_PLAT_ROBOT_ARM_H


/*
 * This UUID is generated online UUID generator
 * https://www.uuidgenerator.net/
 */
// UUID: 846cc0ee-c7f8-48c5-acd8-18bb81cd5dc9
#define TA_ROBOT_ARM_UUID \
	{0x846cc0ee, 0xc7f8, 0x48c5, \
		{ 0xac, 0xd8, 0x18, 0xbb, 0x81, 0xcd, 0x5d, 0xc9} }

/* The function IDs implemented in this TA */
#define TA_PLAT_ROBOT_ARM_CMD_CHECK_GRIP	1
#define TA_PLAT_ROBOT_ARM_CMD_CHECK_HAND	0

/* Some values to check */
#define ROBOT_ARM_GRIP_ANGLE_MIN 15
#define ROBOT_ARM_GRIP_ANGLE_MAX 20

#define ROBOT_ARM_HAND_ANGLE_MIN 40
#define ROBOT_ARM_HAND_ANGLE_MAX 45



#endif /*TA_ROBOT_ARM_H*/
