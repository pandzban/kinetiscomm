/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    tpm.c
 *      Purpose: Microprocessors Laboratory
 *----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/
 
#include "tpm.h"
#include "leds.h"

 void tpmInitialize(void){
	 SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; /* Enable Clock to Port D & E */ 
	 SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	 SIM->SCGC5 |=  SIM_SCGC5_PORTC_MASK;
	 SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3); 
 
	 //CLK = 2MHz
	 
	 ledsInitialize();

	 
	 PORTC->PCR[3] = PORT_PCR_MUX(4);  /* Pin PTC3 is TPM0 CH2 */
	 PORTC->PCR[3] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	 
	 TPM0->SC &= ~TPM_SC_CPWMS_MASK; // ma zliczac w góre
	 TPM0->SC |= TPM_SC_PS(5);   // PS = Fbase * Twy~32us
	 TPM0->CNT = 0xffff; // czyszcze zegar
	 TPM0->SC |= TPM_SC_TOIE_MASK;
	 TPM0->MOD =TPM_MOD_MOD(625-1); // MOD = Fbase/(Prescale * Freq) /// 625-1 dla 10ms
		
	 
	 TPM0->CONTROLS[2].CnSC &= ~TPM_CnSC_MSA_MASK;   // input capture mode
	 TPM0->CONTROLS[2].CnSC &= ~TPM_CnSC_MSB_MASK;
	 TPM0->CONTROLS[2].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK; // rising edge or falling
	

	
	 TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK; // czyszcze flage
	 TPM0->SC |= TPM_SC_CMOD(1); //zlicza zegar wybrany w MCG
	 
	 NVIC_SetPriority(TPM0_IRQn,128);
	 NVIC_ClearPendingIRQ(TPM0_IRQn);
	 NVIC_EnableIRQ(TPM0_IRQn);
 }
 
 
  void TPM0_IRQHandler(void){
	 NVIC_ClearPendingIRQ(TPM0_IRQn);
		if ((TPM0->STATUS & TPM_STATUS_TOF_MASK) && counting){
			czas++;
		}
		if (TPM0->STATUS & TPM_STATUS_CH2F_MASK){   // sprawdzenie kanalu 0 ( event ) 
			PTE->PTOR = led_mask[ledRed];  // swieci sie gdy wcisniety przycisk
	}
	TPM0->STATUS |= TPM_STATUS_TOF_MASK | TPM_STATUS_CH2F_MASK; // czyszczenie flag
 }
	

 

 
 
 
 
 