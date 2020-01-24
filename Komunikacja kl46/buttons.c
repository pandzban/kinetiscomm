/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    buttons.c
 *      Purpose: Microprocessors Laboratory
 *----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/

#include "buttons.h"												//Declarations

/*----------------------------------------------------------------------------
  Function that initializes buttons
 *----------------------------------------------------------------------------*/
void buttonsInitialize(void){

	SIM->SCGC5 |=  SIM_SCGC5_PORTC_MASK; 					/* Enable clock for port C */
	PORTC->PCR[3] |= PORT_PCR_MUX(1);      	/* Pin PTC3 is GPIO */
	PORTC->PCR[12] |= PORT_PCR_MUX(1);      	/* Pin PTC12 is GPIO */
	
	PTC->PDDR &= ~button_mask[button1];  /* enable PTC3 as Input */
	PTC->PDDR &= ~button_mask[button3];  /* enable PTC12 as Input */
	
	PORTC->PCR[3] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* TASK 2.9 - enable on PTC3 pull resistor, and choose correct pull option */
	PORTC->PCR[12] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* TASK 2.9 - enable on PTC12 pull resistor, and choose correct pull option */
}
/*----------------------------------------------------------------------------
	Function reads button 1 state
*----------------------------------------------------------------------------*/
int32_t button1Read(){
	return PTC->PDIR & button_mask[button1];						/* Get port data input register (PDIR) */
}
/*----------------------------------------------------------------------------
	Function reads button 3 state
*----------------------------------------------------------------------------*/
int32_t button3Read(){
	return PTC->PDIR & button_mask[button3];						/* Get port data input register (PDIR) */
}
