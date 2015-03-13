/*
 * File: main.c
 *
 * Copyright (c) 2015, karfes@gmail.com
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 *
 */

#include "main.h"

int main(void){	
	//temporally UART data holder
	uint8_t byte=0, blink=1;

	//initialize system
	SystemInit();

	//initialize UART5 with 8-N-1 settings, 57600 baudrate
	init_uart(UART5_BASE_PTR,periph_clk_khz,57600);

	//clear all interrupts and enable interrupts
	nvic->ICPR[2] |= 1 << (87 & 0x1F); //Clear Pending Interrupts
	nvic->ISER[2] |= 1 << (87 & 0x1F); //Enable Interrupts
	
	//initialize GPIO ports
	gpio_init();

	//Loop forever
	while(1)
	{
		if(blink) toggle_LEDS();
		if(data_available()){
			byte = uart_read();
			if((byte>47 && byte<58)||(byte>64 && byte<71)){	//ascii characters for numbers from 0 to F only
				if(byte<58) display((char)byte-48);			//ascii characters for numbers from 0 to 9 only
				if(byte>58) display((char)byte-55);			//ascii characters for numbers from A to F only
				blink=0;
			}else if(byte==0x7e){	//a tilda(~) send microcontroller information
				display((char)0x00);
				sysinfo();
				blink=0;
			}else{					//new line character plus all others here
				display((char)0x00);
				blink=1;
			}
		}
		delay();
	}
}

/*
	brief  Silly delay
*/
void delay(void)
{
  volatile unsigned int i,j;

  for(i=0; i<25000; i++)
  {
	for(j=0; j<300; j++)
      __asm__("nop");
  }
}
