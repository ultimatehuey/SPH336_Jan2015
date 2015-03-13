/*
 * test.asm
 *
 *  Created on: Mar 9, 2015
 *      Author: karibe
 */

.thumb
.syntax unified

#include "gpio.h"
//IMPORT c


.equ LEDS, 1<<11 | 1<<28 | 1<<29 | 1<<10	//declare the LEDS mask
.equ LED2, 1<<28	//declare LED2 mask

.global toggle_LEDS
.global toggle_LED2

toggle_LEDS:
	STMDB SP!, {R4-R11}	// Push 8 "longs" on the stack and subtracts sp beforehand
	LDR R1, =0x400ff00c	// create a pointer to GPIOA_PTOR
	LDR R2, =LEDS		// load the value of LEDS into R2
	LDR R3, [R1]		// Load GPIOA_PTOR using the pointer
	ORR R3, R2			// mask the value from GPIOA_PTOR with value from c
	STR R3, [R1]		// Store back the
	LDMIA SP!, {R4-R11}	//restores the previous register set from the stack and updates the stack pointer
	MOV R0, #0			//place return value on R0
	MOV PC, LR			//load return address into Program counter


toggle_LED2:
	STMDB SP!, {R4-R11} /* Push 8 "longs" on the stack and subtracts sp beforehand */
	LDR R1, =0x400ff00c// 	create a pointer to GPIOA_PTOR
	LDR R2, =LED2//				load the value of c into R2
	LDR R3, [R1]//			Load GPIOA_PTOR using the pointer
	ORR R3, R2//				mask the value from GPIOA_PTOR with value from c
	STR R3, [R1]//			Store back the
	LDMIA SP!, {R4-R11}
	MOV R0, #0
	MOV PC, LR



