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

#include"gpio.h"
#include"uart.h"
#include "trigger.h"

void delay(void);



int main(void){	
	//temporally UART data holder
	uint8_t byte=0;

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
		//toggle_LED1(); 
		//delay();
		//if(data_available()){
		//	byte = uart_read();
		//}
		//delay();
		//puts((uint8_t*)("Ready\r\n"));
		//LOOP: 
		//delay();
		byte = uart_read();
		//byte2=byte;
		switch (byte) {
			case '0': trigger(0); break;//toggle_LED0(); byte='n'; break;
			case '1': trigger(1); break;//toggle_LED1(); byte='n'; break;
			case '2': trigger(2); break;//toggle_LED2(); byte='n'; break;
			case '3': trigger(3); break;//toggle_LED3(); byte='n'; break;
			case '4': trigger(4); break;//toggle_LED4(); byte='n'; break;
			case '5': trigger(5); break;//toggle_LED5(); byte='n'; break;
			case '6': trigger(6); break;//toggle_LED6(); byte='n'; break;
			case '7': trigger(7); break;//toggle_LED7(); byte='n'; break;
			case '8': trigger(8); break;//toggle_LED8(); byte='n'; break;
			case '9': trigger(9); break;//toggle_LED9(); byte='n'; break;
			default: main(); break;//toggle_LED0(); goto LOOP; break;
		}
		delay();
	}
}





/*
	brief silly delay
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
