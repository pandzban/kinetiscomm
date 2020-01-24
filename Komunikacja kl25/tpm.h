
 
#include "MKL25Z4.h" 
#include "extra.h"

volatile extern uint32_t czas;
volatile extern uint8_t counting;
	
void tpmInitialize(void);
void TPM1_IRQHandler(void);
void TPM2_IRQHandler(void);
void tablica(uint32_t);
