#include "generics.h"

/*--------------------------------
	BASE POINTERS
 --------------------------------*/

//The CPUID Base Register 
//#define cpuidbase 0xE000ED00u

//NIVC base pointer
#define nvicbase 0xE000E100u

//watchdog base pointer
//#define watchdogbase 0x40052000u

//SIM - System Integration Module base pointer
#define simbase 0x40047000u

//MCG - Multipurpose Clock Generator base pointer
#define mcgbase 0x40064000u

//FMC - Flash Memory Controller base pointer
#define fmcbase 0x4001F000u

//LLWU - Low Leakage Wake-Up base pointer
#define llwubase 0x4007C000u

//uart5 base pointer
#define uart5base 0x400EB000u
//Ports base pointers for GPIO
//This is used for R/W, interrupts and so on...
#define porta 0x400FF000u
#define portb 0x400FF040u
#define portc 0x400FF080u
#define portd 0x400FF0C0u
#define porte 0x400FF100u

//Ports base pointers for port peripheral register structure
//This is used for pin control(multiplexing), interrupt status and Filter functionality 
#define pta 0x40049000u
#define ptb 0x4004A000u
#define ptc 0x4004B000u
#define ptd 0x4004C000u
#define pte 0x4004D000u
/*-------------------------------
	END OF BASE POINTERS
  -------------------------------*/



/*--------------------------------
	PERIPHERAL STRUCTURES
 --------------------------------*/

/*--------------------------------
	INTERNALS[GPIO, PORT, WDOG, NVIC, CPUID]
 --------------------------------*/
/** GPIO - Peripheral register structure */
typedef struct {
  volatile REG PDOR;   /**< Port Data Output Register, offset: 0x0 */
  volatile REG PSOR;   /**< Port Set Output Register, offset: 0x4 */
  volatile REG PCOR;   /**< Port Clear Output Register, offset: 0x8 */
  volatile REG PTOR;   /**< Port Toggle Output Register, offset: 0xC */
  volatile REG PDIR;   /**< Port Data Input Register, offset: 0x10 */
  volatile REG PDDR;   /**< Port Data Direction Register, offset: 0x14 */
} volatile *GPIO;

/** PORTs - Ports registers and Register structure*/
//Pin Control Register Structure
typedef struct {
volatile int
ps:1, //Pull Select, 0: internal pull-down resistor, 1: internal pull-up resistor :: Pull Enable(pe below) bit has to be set first
pe:1, //Pull Enable, 0: internal pull-up/down disabled, 1: internal pull-up/down enabled :: Provided pin is configured as a digital input
sre:1, //Slew Rate Enable, 0: fast 1: slow :: pins have to be digital outputs 
pad1:1, //unused
pfe:1, //Passive Filter Enable, 0: Passive Input Filter is disabled, 1: Passive Input Filter is enabled :: Provided pin is configured as a digital input
       //A low pass filter (10 MHz to 30 MHz bandwidth) is enabled on the digital input path, Disable the Passive Input Filter when supporting high speed interfaces (> 2 MHz) on the pin
ode:1, //Open Drain Enable, 0:disabled 1:enabled :: Provided pin is configured as a digital output
dse:1, //Drive Strength Enable, 0:Low drive strength 1:High drive strength :: Provided pin is configured as a digital output  
pad2:1, //unused
mux:3, //mux pins, 000:Pin Disabled (Analog). 001:Alt1(GPIO). 010:Alt2. 011:Alt3. 100:Alt4. 101:Alt5. 110:Alt6. 111:Alt7(chip specific/JTAG/NMI).
pad3:4, //unused
lock:1, //Lock Register, 0:Pin Control Register bits [15:0] are not locked, 1:Pin Control Register bits [15:0] are locked and cannot be updated until the next System Reset
irqc:4, //Interrupt configuration, 0000:Interrupt/DMA Request disabled. 0001:DMA Request on rising edge. 0010:DMA Request on falling edge. 0011:DMA Request on either edge. 0100:Reserved 1000:Interrupt when logic zero. 
	//1001:Interrupt on rising edge. 1010:Interrupt on falling edge. 1011:Interrupt on either edge. 1100:Interrupt when logic one. others:reserved
pad5:4,
isf:1, //Interrupt status flag
pad6:7;
} volatile pcr;

/** PORT - Peripheral register structure */
typedef struct {
  pcr PCR[32];	/**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  REG GPCLR;	/**< Global Pin Control Low Register, offset: 0x80 */
  REG GPCHR;	/**< Global Pin Control High Register, offset: 0x84 */
  uint8_t RESERVED_0[24];
  REG ISFR;	/**< Interrupt Status Flag Register, offset: 0xA0 */
  uint8_t RESERVED_1[28];
  REG DFER;	/**< Digital Filter Enable Register, offset: 0xC0 */
  REG DFCR;	/**< Digital Filter Clock Register, offset: 0xC4 */
  REG DFWR;	/**< Digital Filter Width Register, offset: 0xC8 */
} volatile *PORT;


/** WDOG - Peripheral register structure */
/*typedef struct {
  uint16_t STCTRLH;                                //< Watchdog Status and Control Register High, offset: 0x0 
  uint16_t STCTRLL;                                //< Watchdog Status and Control Register Low, offset: 0x2 
  uint16_t TOVALH;                                 //< Watchdog Time-out Value Register High, offset: 0x4 
  uint16_t TOVALL;                                 //< Watchdog Time-out Value Register Low, offset: 0x6 
  uint16_t WINH;                                   //< Watchdog Window Register High, offset: 0x8 
  uint16_t WINL;                                   //< Watchdog Window Register Low, offset: 0xA 
  uint16_t REFRESH;                                //< Watchdog Refresh Register, offset: 0xC 
  uint16_t UNLOCK;                                 //< Watchdog Unlock Register, offset: 0xE 
  uint16_t TMROUTH;                                //< Watchdog Timer Output Register High, offset: 0x10 
  uint16_t TMROUTL;                                //< Watchdog Timer Output Register Low, offset: 0x12 
  uint16_t RSTCNT;                                 //< Watchdog Reset Count Register, offset: 0x14 
  uint16_t PRESC;                                  //< Watchdog Prescaler Register, offset: 0x16 
} volatile *WDOG;
*/


/** NVIC - Peripheral register structure */
typedef struct {
  uint32_t ISER[4];                                /**< Interrupt Set Enable Register n, array offset: 0x0, array step: 0x4 */
  uint8_t RESERVED_0[112];
  uint32_t ICER[4];                                /**< Interrupt Clear Enable Register n, array offset: 0x80, array step: 0x4 */
  uint8_t RESERVED_1[112];
  uint32_t ISPR[4];                                /**< Interrupt Set Pending Register n, array offset: 0x100, array step: 0x4 */
  uint8_t RESERVED_2[112];
  uint32_t ICPR[4];                                /**< Interrupt Clear Pending Register n, array offset: 0x180, array step: 0x4 */
  uint8_t RESERVED_3[112];
  uint32_t IABR[4];                                /**< Interrupt Active bit Register n, array offset: 0x200, array step: 0x4 */
  uint8_t RESERVED_4[240];
  uint8_t IP[104];                                 /**< Interrupt Priority Register n, array offset: 0x300, array step: 0x1 */
  uint8_t RESERVED_5[2712];
  uint32_t STIR[1];                                /**< Software Trigger Interrupt Register, array offset: 0xE00, array step: 0x4 */
} volatile *NVIC;


/** CPUID - The CPU Identification Number 
*	Implementer(8)|Variant(4)|Constant(4)[=0xF]|PartNo(11)|revision(4) 
*/
/*
typedef struct{
	volatile int 
	revision:4,		//Indicates patch release: 0x1 = Patch 1.
	partno:12,		//Indicates part number: 0xC24 = Cortex-M4
	constant:4,		//Reads as 0xF
	variant:4,		//Indicates part number: 0xC24 = Cortex-M4
	implementer:8;	//Indicates patch release: 0x1 = Patch 1.
} volatile *CPUID;
*/

/*--------------------------------
	END OF INTERNALS[WDOG,NVIC,CPUID]
 --------------------------------*/


/*--------------------------------
	SIM - System Integration Module REGISTERS
 --------------------------------*/
//scgc1
typedef struct{
	volatile int
	padding1:10,	//always zero
	uart4:1,		//UART4 clock gating
	uart5:1,		//UART5 clock gating
	padding2:9,		//always zero
	padding3:1,
	padding4:2,
	padding5:1,
	padding6:7;
} volatile scgc1;

//scgc2
typedef struct{
	volatile int
	enet:1,			//ENET Clock Gate Control
	padding1:11,	//always zero
	dac0:1,			//DAC0 Clock Gate Control
	dac1:1,			//DAC1 Clock Gate Control
	padding:18;		//always zero
} volatile scgc2;

//scgc3
typedef struct{
	volatile int
	rngb:1,		//RNGB Clock Gate Control
	padding1:3,	//always zero
	flexcan1:1,	//FlexCAN1 Clock Gate Control
	padding2:7,	//always zero
	spi2:1,		//SPI2 Clock Gate Control
	padding3:4,	//always zero
	sdhc:1,		//SDHC Clock Gate Control
	padding4:6,	//always zero
	ftm2:1,		//FTM2 Clock Gate Control
	padding5:2,	//always zero
	adc1:1,		//ADC1 Clock Gate Control
	padding6:2,
	padding7:1,
	padding8:1;
} volatile scgc3;

//scgc4
typedef struct{
	volatile int
	padding1:1,	//always zero
	ewm:1,		//EWM Clock Gate Control
	cmt:1,		//CMT Clock Gate Control
	padding2:1,	//always zero
	padding3:2,	//always one
	i2c0:1,		//I2C0 Clock Gate Control
	i2c1:1,		//I2C1 Clock Gate Control
	padding4:1,	//always zero
	uart0:1,	//UART0 Clock Gate Control
	uart1:1,	//UART1 Clock Gate Control
	uart2:1,	//UART2 Clock Gate Control
	uart3:1,	//UART3 Clock Gate Control
	padding5:4,	//always zero
	usbotg:1,	//USB Clock Gate Control
	cmp:1,		//Comparator  Clock Gate Control
	vref:1,		//Vref Clock Gate Control
	padding6:7,	//always zero
	llwu:1,		//LLWU Clock Gate Control
	padding7:2,	//always one
	padding8:1;	//always zero
} volatile scgc4;
//SCGC5
typedef struct{
	volatile int
	lptimer:1, 		//Low Power Timer Clock Gate Control
	regfile:1, 		//Register File Clock Gate Control
	padding1:3, 	//always zero
	tsi:1, 			//TSI Clock Gate Control
	padding2:1, 	//always zero
	padding3:2,		//always one
	portA:1,		//Port A Clock Gate Control
	portB:1,		//Port A Clock Gate Control
	portC:1,		//Port A Clock Gate Control
	portD:1,		//Port A Clock Gate Control
	portE:1,		//Port A Clock Gate Control
	padding4:4,		//always zero
	padding5:1,		//always one
	padding6:13;	//always zero
} volatile scgc5;

typedef struct {
	volatile int
	ftfl:1,		//Flash Memory Clock Gate Control
	dmamux:1,	//DMA Mux Clock Gate Control
	padding1:2,	//always zero
	flexcan0:1,	//FlexCAN0 Clock Gate Control
	padding2:7,	//always zero
	spi0:1,		//SPI0 Clock Gate Control
	spi1:1,		//SPI1 Clock Gate Control
	padding3:1,	//always zero
	i2s:1,		//I2S Clock Gate Control
	padding4:2,	//always zero
	crc:1,		//CRC Clock Gate Control
	padding5:2,	//always zero
	usbdcd:1,	//USB DCD Clock Gate Control
	pdb:1,		//PDB Clock Gate Control
	pit:1,		//PIT Clock Gate Control
	ftm0:1,		//FTM0 Clock Gate Control
	ftm1:1,		//FTM1 Clock Gate Control
	padding6:1,	//always zero
	adc0:1,		//ADC0 Clock Gate Control
	padding7:1,	//always zero
	rtc:1,		//RTC Clock Gate Control
	padding8:1,	//always one
	padding9:1;	//always zero
} volatile scgc6;

typedef struct{
	volatile int
	flexbus:1,	//FlexBus Clock Gate Control
	dma:1,		//DMA Clock Gate Control
	mpu:1,		//MPU Clock Gate Control
	padding:29;
}volatile scgc7;

typedef struct{
	volatile int
	padding:16,	//always zero
	outdiv4:4,	//divide value for flash clock[0000=divide by 1, 0001=divide by 2,... 1111=divide by 16]
	outdiv3:4,	//FlexBus clock driven to the external pin (FB_CLK)[0000=divide by 1, 0001=divide by 2,... 1111=divide by 16]
	outdiv2:4,	//divide value for peripheral clock[0000=divide by 1, 0001=divide by 2,... 1111=divide by 16]
	outdiv1:4;	//divide value for the core/system clock[0000=divide by 1, 0001=divide by 2,... 1111=divide by 16]
} volatile clkdiv1;

/** SIM - Peripheral register structure */
typedef struct{
  uint32_t SOPT1;                                  /**< System Options Register 1, offset: 0x0 */
  uint8_t RESERVED_0[4096];
  uint32_t SOPT2;                                  /**< System Options Register 2, offset: 0x1004 */
  uint8_t RESERVED_1[4];
  uint32_t SOPT4;                                  /**< System Options Register 4, offset: 0x100C */
  uint32_t SOPT5;                                  /**< System Options Register 5, offset: 0x1010 */
  uint32_t SOPT6;                                  /**< System Options Register 6, offset: 0x1014 */
  uint32_t SOPT7;                                  /**< System Options Register 7, offset: 0x1018 */
  uint8_t RESERVED_2[8];
  uint32_t SDID;                                   /**< System Device Identification Register, offset: 0x1024 */
  scgc1 SCGC1;                                  /**< System Clock Gating Control Register 1, offset: 0x1028 */
  scgc2 SCGC2;                                  /**< System Clock Gating Control Register 2, offset: 0x102C */
  scgc3 SCGC3;                                  /**< System Clock Gating Control Register 3, offset: 0x1030 */
  scgc4 SCGC4;                                  /**< System Clock Gating Control Register 4, offset: 0x1034 */
  scgc5 SCGC5;                                  /**< System Clock Gating Control Register 5, offset: 0x1038 */
  scgc6 SCGC6;                                  /**< System Clock Gating Control Register 6, offset: 0x103C */
  scgc7 SCGC7;                                  /**< System Clock Gating Control Register 7, offset: 0x1040 */
  clkdiv1 CLKDIV1;                                /**< System Clock Divider Register 1, offset: 0x1044 */
  uint32_t CLKDIV2;                                /**< System Clock Divider Register 2, offset: 0x1048 */
  uint32_t FCFG1;                                  /**< Flash Configuration Register 1, offset: 0x104C */
  uint32_t FCFG2;                                  /**< Flash Configuration Register 2, offset: 0x1050 */
  uint32_t UIDH;                                   /**< Unique Identification Register High, offset: 0x1054 */
  uint32_t UIDMH;                                  /**< Unique Identification Register Mid-High, offset: 0x1058 */
  uint32_t UIDML;                                  /**< Unique Identification Register Mid Low, offset: 0x105C */
  uint32_t UIDL;                                   /**< Unique Identification Register Low, offset: 0x1060 */
} volatile *SIM;
/*--------------------------------
	END OF SIM
 --------------------------------*/


/* ------------------------------
   -- MCG
   ---------------------------- */
//c1
typedef struct{
	volatile uint8_t
	irefsten:1,		//Internal Reference Stop Enable
	irclken:1,		//Internal Reference Clock Enable
	irefs:1,		//Internal Reference Select
	frdiv:3,		//FLL External Reference Divider
	clks:2;			//Clock Source Select
} volatile c1;

typedef union{
	c1 bit_reg;
	uint8_t byte_reg;
} dualc1;

//c2
typedef struct{
	volatile uint8_t
	ircs:1,		//Internal Reference Clock Select
	lp:1,		//Low Power Select
	erefs:1,	//External Reference Select
	hgo:1,		//High Gain Oscillator Select
	range:2,	//Frequency Range Select
	padding1:1,	//reserved
	padding2:1;	//reserved	
} volatile c2;

typedef union{
	c2 bit_reg;
	uint8_t byte_reg;
}dualc2;

typedef struct{
	volatile uint8_t 
	prdiv:5,	//PLL External Reference Divider[00000=divide by 1, 00001=divide by 2..... 11000=divide by 25]
	pllsten:1,	//PLL Stop Enable[Enables the PLL Clock during Normal Stop, but not in Low Power Mode]
	pllclken:1,	//PLL Clock Enable[Enables the PLL independent of PLLS and enables the PLL clock for use as MCGPLLCLK]
				//PRDIV needs to be programmed to the correct divider to generate a PLL reference clock in the range of 2 - 4MHz range prior to setting the PLLCLKEN bit
				//the OSCINIT bit should be checked to makesure it is set
	padding:1;
} volatile c5;

typedef struct{
	volatile int
	vdiv:5,		//VCO Divider
	cme:1,		//Clock Monitor Enable
	plls:1,		//PLL Select
	lolie:1;	//Loss of lock interrupt enable
} volatile c6;

typedef union{
	c6 bit_reg;
	uint8_t byte_reg;
} dualc6;

typedef struct{
	volatile uint8_t 
	ircst:1,	//Internal Reference Clock Status[indicates the current source for the internal reference clock select clock (IRCSCLK)]
	oscinit:1,	//OSC Initialization, set to 1 when OSC init cycles are completed
	clkst:2,	//Clock Mode Status [indicate the current clock mode, not immediately after a write to the CLKS]
	irefst:1,	//Internal Reference Status[indicates the current source for the FLL reference clock, not immediately after a write to the IREFS]
	pllst:1,	//PLL Select Status[indicates the clock source selected by PLLS, not immediately after a write to the PLLS]
	lock:1,		//Lock Status[indicates whether the PLL has acquired lock, ]
	lols:1;		//Loss of Lock Status
} volatile s;

/** MCG - Peripheral register structure */
typedef struct{
  dualc1 C1;				/**< MCG Control 1 Register, offset: 0x0 */
  dualc2 C2;			/**< MCG Control 2 Register, offset: 0x1 */
  uint8_t C3;			/**< MCG Control 3 Register, offset: 0x2 */
  uint8_t C4;			/**< MCG Control 4 Register, offset: 0x3 */
  c5 C5;			/**< MCG Control 5 Register, offset: 0x4 */
  dualc6 C6;			/**< MCG Control 6 Register, offset: 0x5 */
  s S;					/**< MCG Status Register, offset: 0x6 */
  uint8_t RESERVED_0[1];
  uint8_t ATC;			/**< MCG Auto Trim Control Register, offset: 0x8 */
  uint8_t RESERVED_1[1];
  uint8_t ATCVH;		/**< MCG Auto Trim Compare Value High Register, offset: 0xA */
  uint8_t ATCVL;		/**< MCG Auto Trim Compare Value Low Register, offset: 0xB */
} volatile *MCG;

/*--------------------------------
	END OF MCG
 --------------------------------*/

/* ----------------------------------------------------------------------------
   -- LLWU PERIPHERAL
   ---------------------------------------------------------------------------- */
//CS 
typedef struct{
	volatile int
	fltr:1,		//Digital Filter on RESET Pin
	fltep:1,	//Digital Filter on External Pin
	padding1:1,	//reserved
	padding2:4,	//always zero
	ackiso:1;	//Acknowledge Isolation
} volatile cs;

/** LLWU - Peripheral register structure */
typedef struct{
  uint8_t PE1;                                     /**< LLWU Pin Enable 1 Register, offset: 0x0 */
  uint8_t PE2;                                     /**< LLWU Pin Enable 2 Register, offset: 0x1 */
  uint8_t PE3;                                     /**< LLWU Pin Enable 3 Register, offset: 0x2 */
  uint8_t PE4;                                     /**< LLWU Pin Enable 4 Register, offset: 0x3 */
  uint8_t ME;                                      /**< LLWU Module Enable Register, offset: 0x4 */
  uint8_t F1;                                      /**< LLWU Flag 1 Register, offset: 0x5 */
  uint8_t F2;                                      /**< LLWU Flag 2 Register, offset: 0x6 */
  uint8_t F3;                                      /**< LLWU Flag 3 Register, offset: 0x7 */
  cs CS;                                      /**< LLWU Control and Status Register, offset: 0x8 */
} volatile *LLWU;
/*--------------------------------
	END OF LLWU
 --------------------------------*/
/* ------------------------------
   -- FMC
   ---------------------------- */

/** FMC - Peripheral register structure */
typedef struct{
  uint32_t PFAPR;                                  /**< Flash Access Protection Register, offset: 0x0 */
  uint32_t PFB0CR;                                 /**< Flash Bank 0 Control Register, offset: 0x4 */
  uint32_t PFB1CR;                                 /**< Flash Bank 1 Control Register, offset: 0x8 */
  uint8_t RESERVED_0[244];
  uint32_t TAGVD[4][8];                            /**< Cache Directory Storage, array offset: 0x100, array step: index*0x20, index2*0x4 */
  uint8_t RESERVED_1[128];
  struct {                                         /* offset: 0x200, array step: index*0x40, index2*0x8 */
    uint32_t DATA_U;                                 /**< Cache Data Storage (upper word), array offset: 0x200, array step: index*0x40, index2*0x8 */
    uint32_t DATA_L;                                 /**< Cache Data Storage (lower word), array offset: 0x204, array step: index*0x40, index2*0x8 */
  } SET[4][8];
} volatile *FMC;

/*--------------------------------
	END OF FMC
 --------------------------------*/

typedef struct{
	volatile int
	sc16p:1,	//Oscillator 16 pF Capacitor Load Configure
	sc8p:1,		//Oscillator 8 pF Capacitor Load Configure
	sc4p:1,
	sc2p:1,
	padding1:1,
	erefsten:1,	//External Reference Stop Enable
	padding2:1,
	erclken:1;
} volatile *OSCI_CR;


/** UART - Peripheral register structure */
typedef struct{
  uint8_t BDH;                                     /**< UART Baud Rate Registers:High, offset: 0x0 */
  uint8_t BDL;                                     /**< UART Baud Rate Registers: Low, offset: 0x1 */
  uint8_t C1;                                      /**< UART Control Register 1, offset: 0x2 */
  uint8_t C2;                                      /**< UART Control Register 2, offset: 0x3 */
  uint8_t S1;                                      /**< UART Status Register 1, offset: 0x4 */
  uint8_t S2;                                      /**< UART Status Register 2, offset: 0x5 */
  uint8_t C3;                                      /**< UART Control Register 3, offset: 0x6 */
  uint8_t D;                                       /**< UART Data Register, offset: 0x7 */
  uint8_t MA1;                                     /**< UART Match Address Registers 1, offset: 0x8 */
  uint8_t MA2;                                     /**< UART Match Address Registers 2, offset: 0x9 */
  uint8_t C4;                                      /**< UART Control Register 4, offset: 0xA */
  uint8_t C5;                                      /**< UART Control Register 5, offset: 0xB */
  uint8_t ED;                                      /**< UART Extended Data Register, offset: 0xC */
  uint8_t MODEM;                                   /**< UART Modem Register, offset: 0xD */
  uint8_t IR;                                      /**< UART Infrared Register, offset: 0xE */
  uint8_t RESERVED_0[1];
  uint8_t PFIFO;                                   /**< UART FIFO Parameters, offset: 0x10 */
  uint8_t CFIFO;                                   /**< UART FIFO Control Register, offset: 0x11 */
  uint8_t SFIFO;                                   /**< UART FIFO Status Register, offset: 0x12 */
  uint8_t TWFIFO;                                  /**< UART FIFO Transmit Watermark, offset: 0x13 */
  uint8_t TCFIFO;                                  /**< UART FIFO Transmit Count, offset: 0x14 */
  uint8_t RWFIFO;                                  /**< UART FIFO Receive Watermark, offset: 0x15 */
  uint8_t RCFIFO;                                  /**< UART FIFO Receive Count, offset: 0x16 */
  uint8_t RESERVED_1[1];
  uint8_t C7816;                                   /**< UART 7816 Control Register, offset: 0x18 */
  uint8_t IE7816;                                  /**< UART 7816 Interrupt Enable Register, offset: 0x19 */
  uint8_t IS7816;                                  /**< UART 7816 Interrupt Status Register, offset: 0x1A */
  union {                                          /* offset: 0x1B */
    uint8_t WP7816_T_TYPE0;                          /**< UART 7816 Wait Parameter Register, offset: 0x1B */
    uint8_t WP7816_T_TYPE1;                          /**< UART 7816 Wait Parameter Register, offset: 0x1B */
  };
  uint8_t WN7816;                                  /**< UART 7816 Wait N Register, offset: 0x1C */
  uint8_t WF7816;                                  /**< UART 7816 Wait FD Register, offset: 0x1D */
  uint8_t ET7816;                                  /**< UART 7816 Error Threshold Register, offset: 0x1E */
  uint8_t TL7816;                                  /**< UART 7816 Transmit Length Register, offset: 0x1F */
} volatile *UART;

/*--------------------------------
	Peripheral instances
 --------------------------------*/
//instances of ports
PORT PA = (PORT)pta;
//PORT portB = (PORT)ptb;
//PORT portC = (PORT)ptc;
//PORT portD = (PORT)ptd;
PORT PE = (PORT)pte;

//instances of GPIO for ports
GPIO GPIOA = (GPIO)porta;
//GPIO PTB = (GPIO)portb;
//GPIO PTC = (GPIO)portc;
//GPIO PTD = (GPIO)portd;
GPIO GPIOE = (GPIO)porte;

//mcg instance 
MCG mcg = (MCG)mcgbase;

//llwu instance
LLWU llwu = (LLWU)llwubase;

//sim instance 
SIM sim = (SIM)simbase;

//cpuid instance
//CPUID cpuid = (CPUID)cpuidbase;

//nivc instance
NVIC nvic = (NVIC)nvicbase;

//FMC instance
FMC fmc = (FMC)fmcbase;

OSCI_CR osci = (OSCI_CR)0x40065000u;

UART uart5 = (UART)uart5base;

