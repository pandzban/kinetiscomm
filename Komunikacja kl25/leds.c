

#include "leds.h"												//Declarations

#define RED_LED_SHIFT 18
#define GREEN_LED_SHIFT 19
#define BLUE_LED_SHIFT 1
#define MASK(x) (1ul << (x))

const uint32_t red_mask= 1UL<<5;				//Red led is Port D bit 5
const uint32_t green_mask= 1UL<<29;			//Green led is Port C bit 5/

/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void ledsInitialize(void){
	volatile int delay;
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	PORTB->PCR[RED_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_SHIFT] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED_SHIFT] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED_SHIFT] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PDDR |= MASK(BLUE_LED_SHIFT);
	PTB->PCOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);

}
/*----------------------------------------------------------------------------
  Function that blinks (x) times green led with (y) delay in mc
 *----------------------------------------------------------------------------*/
void ledgreenBlink(uint32_t x, uint32_t y){
	uint32_t count;
	
	for(count = 0; count < x; count++){	
		PTB->PCOR |= MASK(GREEN_LED_SHIFT);
		delay_mc(y);		
		PTB->PSOR |= MASK(GREEN_LED_SHIFT);
		delay_mc(y);
	}
}
/*----------------------------------------------------------------------------
  Function that blinks (x) times red led with (y) delay in mc
 *----------------------------------------------------------------------------*/
void ledredBlink(uint32_t x, uint32_t y){
	uint32_t count;
	
	for(count = 0; count < x; count++){	
		PTB->PCOR |= MASK(RED_LED_SHIFT);
		delay_mc(y);		
		PTB->PSOR |= MASK(RED_LED_SHIFT	);
		delay_mc(y);	
	}
}

void ledblueBlink(uint32_t x, uint32_t y){
	uint32_t count;
	
	for(count = 0; count < x; count++){	
		PTD->PCOR |= MASK(BLUE_LED_SHIFT);
		delay_mc(y);		
		PTD->PSOR |= MASK(BLUE_LED_SHIFT	);
		delay_mc(y);	
	}
}

void customBlink(uint32_t x, uint32_t y){
	uint32_t count;
		for(count = 0; count < x; count++){	
		PTD->PCOR |= MASK(BLUE_LED_SHIFT);
		delay_mc(y);		
			PTB->PCOR |= MASK(RED_LED_SHIFT);
		delay_mc(y);	
						PTD->PCOR |= MASK(GREEN_LED_SHIFT);
		delay_mc(y);		

		PTD->PCOR |= MASK(BLUE_LED_SHIFT	);
		delay_mc(y);	
						PTD->PSOR |= MASK(RED_LED_SHIFT	);
		delay_mc(y);

			PTB->PSOR |= MASK(GREEN_LED_SHIFT	);
		delay_mc(y);
	}
	
}
/*----------------------------------------------------------------------------
  Function that turns all LEDs off
 *----------------------------------------------------------------------------*/
void ledsOff(void) {
	PTB->PSOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PSOR |= MASK(BLUE_LED_SHIFT);
}
/*----------------------------------------------------------------------------
  Function that turns all LEDs on
 *----------------------------------------------------------------------------*/
void ledsOn(void) {
	PTB->PCOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);   	/* switch Green LED on */
}
