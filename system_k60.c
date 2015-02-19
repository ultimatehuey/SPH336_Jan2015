/*
 * File: system_k60.c
 *
 * Copyright (c) 2013, 0xc0170, edited by karibe
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 *
 */
#include "MK60DZ10.h"
void SystemInit(void);
void init_uart(UART_MemMapPtr uartch, int sysclk, int baud);
void uart_putchar (UART_MemMapPtr channel, uint8_t ch);
void uartsend(uint8_t ch);
void puts(uint8_t *s);
char uart_read(void);
uint16_t data_ready(void);
uint16_t uart_getchar_present (UART_MemMapPtr channel);
uint8_t uart_getchar (UART_MemMapPtr channel);

int core_clk_mhz = 96;
int periph_clk_khz;

/**
 * \brief  Clock initialization (96 MHz)
 *
 * \param  void
 *
 * \return void
 */
void SystemInit(void)
{
  uint32_t temp_pfapr, i;

  SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK
              | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK
              | SIM_SCGC5_PORTE_MASK);
  SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;	//uart5 enabled
  MCG_C2 = 0;
  SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
  LLWU_CS |= LLWU_CS_ACKISO_MASK;
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
  while (MCG_S & MCG_S_IREFST_MASK);
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2);

  MCG_C5 = MCG_C5_PRDIV(0x18);
  MCG_C6 = 0x0;

  temp_pfapr = FMC_PFAPR;

  FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
             | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
             | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;

  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1)
              | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);

  for (i = 0 ; i < 4 ; i++);

  FMC_PFAPR = temp_pfapr;

  MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24);
  while (!(MCG_S & MCG_S_PLLST_MASK));

  while (!(MCG_S & MCG_S_LOCK_MASK));
  MCG_C1 &= ~MCG_C1_CLKS_MASK;
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);

  periph_clk_khz = 96000 / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24) + 1);
  init_uart(UART5_BASE_PTR,periph_clk_khz,38400);
}

void init_uart(UART_MemMapPtr uartch, int sysclk, int baud){
	uint16_t ubd, temp, brfa;
	/* Make sure that the transmitter and receiver are disabled while we
	* change settings.
	*/
	UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	/* Configure the UART for 8-bit mode, no parity */
	/* We need all default settings, so entire register is cleared */
	UART_C1_REG(uartch) = 0;
	/* Configure the UART for 8-bit mode, no parity */
	/* We need all default settings, so entire register is cleared */
	UART_C1_REG(uartch) = 0;
	/* Calculate baud settings */
	ubd = (uint16_t)((sysclk*1000)/(baud * 16));
	/* Save off the current value of the UARTx_BDH except for the SBR */
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));
	UART_BDH_REG(uartch) = temp | UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8_t)(ubd & UART_BDL_SBR_MASK);
	/* Determine if a fractional divider is needed to get closer to the baud rate */
	brfa = (((sysclk*32000)/(baud * 16)) - (ubd * 32));
	/* Save off the current value of the UARTx_C4 register except for the BRFA */
	temp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1F));
	UART_C4_REG(uartch) = temp | UART_C4_BRFA(brfa);
	/* Enable receiver and transmitter */
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
	//UART_S1_REG(uartch) &= ~UART_S1_TDRE_MASK;
}

void uart_putchar (UART_MemMapPtr channel, uint8_t ch)
{
	/* Wait until space is available in the FIFO */
	while(!(UART_S1_REG(channel) & UART_S1_TDRE_MASK));

	/* Send the character */
	UART_D_REG(channel) = (uint8_t)ch;
}

uint8_t uart_getchar (UART_MemMapPtr channel)
{
	/* Wait until character has been received */
	while (!(UART_S1_REG(channel) & UART_S1_RDRF_MASK));

	/* Return the 8-bit data from the receiver */
	return (uint8_t)UART_D_REG(channel);
}

uint16_t uart_getchar_present (UART_MemMapPtr channel)
{
	return (uint16_t)(UART_S1_REG(channel) & UART_S1_RDRF_MASK);
}

uint16_t data_ready(void){
	return uart_getchar_present (UART5_BASE_PTR);
}

char uart_read(void){
	return uart_getchar (UART5_BASE_PTR);
}

//send a char to UART5_TX
void uartsend(uint8_t ch){
	uart_putchar (UART5_BASE_PTR,ch);
}

//send a string
void puts(uint8_t *s){
	while(*s!='\0'){
		uartsend(*s++);
	}
}

