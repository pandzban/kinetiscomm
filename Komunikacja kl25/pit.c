
 
#include "pit.h"


	//CLK = 1MHz
#define COUNT_FREQ 1000000

#define PIT_FREQ1 1000 // in Hertz
#define PIT_FREQ2 1000

#define PIT_IRQ (IRQn_Type) 22	/* zamiast PIT_IRQn */


volatile uint32_t time = 0;
volatile uint8_t calculate = 0;

void systickInit(void){
	  NVIC_SetPriority(SysTick_IRQn,128);
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
   
    SysTick_Config(SystemCoreClock / 100);      /* Configure SysTick to generate an interrupt every millisecond */
}

void SysTick_Handler(void)  {                            /* SysTick interrupt Handler. */
    NVIC_ClearPendingIRQ(SysTick_IRQn);
	//ledredBlink(1,15);
    calculate = 1;                          /* See startup file startup_LPC17xx.s for SysTick vector */
}

void pitInitialize(void){     

	int LDVAL1 = COUNT_FREQ / PIT_FREQ1 - 1 ;
	int LDVAL2 = COUNT_FREQ / PIT_FREQ2 - 1 ;
	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; // enabled by writing 0
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	//PIT->CHANNEL[0].LDVAL = 0x123456;//PIT_LDVAL_TSV(starting_value);
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(LDVAL1); // zdefiniowana w .h
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK; // no chain
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // generate interrupts for channel 0
	
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(LDVAL2);
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK;
	//PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;

	NVIC_ClearPendingIRQ(PIT_IRQn);  	/* Clear any pending interrupt */
	NVIC_EnableIRQ(PIT_IRQn);			/* Notify Nested Vector Interrupt Controller */
//	NVIC_SetPriority(PIT_IRQn, 128); 	/* ToDo 7.2b Enable NVIC interrupts source for lptimer module */ /* ToDo 7.3: Increase lptimer interrupt priority level  */
	__enable_irq();
	
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer channel
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer channel
	PIT->MCR &= ~PIT_MCR_FRZ_MASK; // disable freeze
}


void PIT_IRQHandler(void){
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) { //All interrupts can be enabled or masked by setting TCTRLn[TIE]. A new interrupt can be
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK; // czysci flage
	}
 if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
	 		time++; 
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}
}

uint32_t get_pit_time(void){
	return time;
}
