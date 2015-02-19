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
#include "Mk60.h"

extern void SystemInit(void);
extern void uartsend(uint8_t ch);
extern void puts(uint8_t *s);
extern char uart_read(void);
extern uint16_t data_ready(void);


void gpio_init(void);
void delay(void);
void toggle_LED1(void);
void toggle_LED2(void);


int main(void){	
	//initialize system
	char byte=0, byte2;
	SystemInit();
	
	//clear all interrupts and enable interrupts
	nvic->ICPR[2] |= 1 << (87 & 0x1F); //NVICICPR2 |= 1 << (87 & 0x1F);//Interrupt Clear Pending Register
	nvic->ISER[2] |= 1 << (87 & 0x1F); //NVICISER2 |= 1 << (87 & 0x1F);//Interrupt Set Enable Register
	
	//initialize GPIO ports
	gpio_init();
	//infinite loop
	while(1)
	{
		toggle_LED1(); 
		delay();
		if(data_ready){
			byte = uart_read();
			byte2=byte;
		}
		delay();
	}
}

/**
 * \brief PORTA initialization
 */
void gpio_init(void)
{
	//PTA19 as a button input
	PA->PCR[19].mux = 0x01; //PortA pin 19 as GPIO
	PA->PCR[19].irqc = 0xA; //Interrupt on falling edge
	PA->PCR[19].pe = 0x01; //pull-up enable
	PA->PCR[19].ps = 0x01; //select internal pull-up resistor
	//PortA pins 10, 11, 28 and 29 as alt1=GPIO
	PA->PCR[10].mux = 0x01;
	PA->PCR[11].mux = 0x01;
	PA->PCR[28].mux = 0x01;
	PA->PCR[29].mux = 0x01;
	//PTE8 and PTE9 as UART5
	PE->PCR[8].mux = 0x0;	//clear default function
	PE->PCR[9].mux = 0x0;	//clear default function
	PE->PCR[8].mux = 0x3;	//alt3 = UART5_TX
	PE->PCR[9].mux = 0x3; 	//alt3 = UART5_RX
	//GPIO port data direction Port A as output for LEDs (pin 11, 28, 29 and 10), Port E UART5(PTE8 TX, PTE9 RX)
	GPIOA->PDDR.bit_reg.bit11 = OUT;
	GPIOA->PDDR.bit_reg.bit28 = OUT;
	GPIOA->PDDR.bit_reg.bit29 = OUT;
	GPIOA->PDDR.bit_reg.bit10 = OUT;
	GPIOE->PDDR.bit_reg.bit8 = OUT; //UART5_TX is an output
	//No need to configure GPIO for as an input, by default all pins are inputs
	//GPIOA->PDDR.bit_reg.bit19 = IN;
	//GPIOE->PDDR.bit_reg.bit9 = IN //UART5_RX is an input
}



void toggle_LED1(void){
	GPIOA->PTOR.bit_reg.bit11 = ON;
	GPIOA->PTOR.bit_reg.bit29 = ON;
}

void toggle_LED2(void){
	GPIOA->PTOR.bit_reg.bit28 = ON;
	GPIOA->PTOR.bit_reg.bit10 = ON;
	puts((uint8_t*)("Hello World\r\n"));
}


/**
 * \brief  Port A ISR Handler
 */
void PORTA_IRQHandler(void)
{
	PA->ISFR.word_reg = 0xFFFFFFFF; //clear Interrupt Status Register by writing ones in all bits --- why???
	toggle_LED2(); //toggle the second LED to indicate interrupt serviced
}

/**
 * \brief  Silly delay
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
