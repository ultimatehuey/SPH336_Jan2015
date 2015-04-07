/*
 * ledblink.h
 *
 *  Created on: Apr 2, 2015
 *      Author: bonny
 */

#ifndef LEDBLINK_H_
#define LEDBLINK_H_
#ifndef TRIGGER_H_
#define TRIGGER_H_
#include <stdio.h>

void  trigger(int byte1){
		//int  E1, E2, E3, E4;

	  //byte1 = byte_checker();
	  int n, c, k;
	  int LED[4];

	 n = byte1;
	  for (c = 3; c >= 0; c--)
	  {
	    k = n >> c;

	    if (k & 1)
	    	LED[c] = 1;
	    else
	    	LED[c] = 0;
	  }
	  /*E1 = LED[0];
	  E2 = LED[1];
	  E3 = LED[2];
	  E4 = LED[3];*/

	 	/* if (E1==1){
		  	GPIOA->PDDR.bit_reg.bit11 = on;}
	  	else{
	  		 GPIOA->PDDR.bit_reg.bit11 = off;}

	  	if (E2==1){
	  		 GPIOA->PDDR.bit_reg.bit28 = on;}
	  	else{
	  		  GPIOA->PDDR.bit_reg.bit28 = off;}

	  	if (E3==1){
	  		  GPIOA->PDDR.bit_reg.bit29 = on;}
	  	else{
	  		  GPIOA->PDDR.bit_reg.bit29 = off; }

	  	if (E4==1){
	  		  GPIOA->PDDR.bit_reg.bit10 = on; }
	  	else{
	  		  GPIOA->PDDR.bit_reg.bit10 = off; }*/
                GPIOA->PDDR.bit_reg.bit11 = LED[0];
		GPIOA->PDDR.bit_reg.bit28 = LED[1];
		GPIOA->PDDR.bit_reg.bit29 = LED[2];
		GPIOA->PDDR.bit_reg.bit10 = LED[3];
		//puts((uint8_t*)(n,"\r\n"));
}





#endif /* LEDBLINK_H_ */
