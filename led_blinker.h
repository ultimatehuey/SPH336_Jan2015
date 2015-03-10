/*
 * led_blinker.h
 *
 *  Created on: Mar 6, 2015
 *      Author: gathu
 */

#ifndef LED_BLINKER_H_
#define LED_BLINKER_H_
#include <stdio.h>
//#include "Mk60.h"
#include "gpio.h"

void led_blinker(char byte1);


void  led_blinker(char byte1){
	  int i,k;
	  int LED[4];
	  for (i = 0; i < 4; i++) {
	  k = (( byte1 << i));

	   if (k & 0x08){
	    	LED[i] = 1;}
	    else{
	    	LED[i] = 0;}
	  }
        //printf("%d  %d  %d  %d", A[4], A[5], A[6],A[7]);
	    GPIOA->PDDR.bit_reg.bit11 = LED[0]; //e1
	  	GPIOA->PDDR.bit_reg.bit28 = LED[1]; //e2
	  	GPIOA->PDDR.bit_reg.bit29 = LED[2]; //e3
	  	GPIOA->PDDR.bit_reg.bit10 = LED[3]; //e4

}

#endif /* LED_BLINKER_H_ */
