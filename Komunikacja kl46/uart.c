
 
 #include "uart.h"
 #include "send.h"
 #define BUS_CLOCK 24e6
 #define XTAL 8e6
 
 uint32_t baud_rate0 = 9600; // checked up to 19200 // needs to match with PC config
 uint32_t baud_rate1 = 9600;   // can be changed
 uint32_t baud_rate2 = 9600;
 
 void uart12Initialize(void){
 uint32_t divisor = 0;
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // porty dla UART
	PORTE->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[16] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[17] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[0] |= PORT_PCR_MUX(3); // ALT3 = UART
	PORTE->PCR[1] |= PORT_PCR_MUX(3);
	PORTE->PCR[16] |= PORT_PCR_MUX(3);
	PORTE->PCR[17] |= PORT_PCR_MUX(3);
	UART1->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );	
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );	
	divisor = BUS_CLOCK/(baud_rate1*16);
	UART1->BDH = UART_BDH_SBR(divisor>>8);
	UART1->BDL = UART_BDL_SBR(divisor);
	divisor = BUS_CLOCK/(baud_rate2*16);
	UART2->BDH = UART_BDH_SBR(divisor>>8);
	UART2->BDL = UART_BDL_SBR(divisor);
	UART1->BDH &= ~UART_BDH_SBNS_MASK; // 1 bit stopu
	UART2->BDH &= ~UART_BDH_SBNS_MASK;
	UART1->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);	
	UART2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);	
}

 void uart0Initialize(void){
	  uint32_t divisor = 0;
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//UART0 dolaczony do zegara
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;		//Port A dolaczony do zegara
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(2);
	PORTA->PCR[1] = PORT_PCR_MUX(2u);			//PTA1=RX_D
	PORTA->PCR[2] = PORT_PCR_MUX(2u);			//PTA2=TX_D
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );		//Blokada nadajnika i o dbiornika //
	divisor = XTAL/(baud_rate0*32);
	UART0->BDH = UART_BDH_SBR(divisor>>8); //
	UART0->BDL = UART_BDL_SBR(divisor);
	UART0->BDH &= ~UART0_BDH_SBNS_MASK; // 1 bit stopu zamiast dwóch
	UART0->C4 |= UART0_C4_OSR(31);
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK;
	UART0->C2 |= UART_C2_RIE_MASK | UART_C2_TIE_MASK;//UART0_C2_TIE_MASK;//UART_C2_RIE_MASK; //| UART_C2_TIE_MASK;
	UART0->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);		//Wlacz nadajnik i o dbiornik //	
	NVIC_ClearPendingIRQ(UART0_IRQn);
 }
 
	void UART0_IRQHandler() {
		if (uart0_state == send){
			if (UART0->S1 & UART0_S1_RDRF_MASK){
				received = UART0->D;
				UART0->C2 &= ~UART0_C2_RIE_MASK;
			}
			if ((UART0->C2 & UART_C2_TIE_MASK) && (UART0->S1 & UART0_S1_TDRE_MASK)){
				if (!QS_Empty(&SndBuff)){
					QS_Send(&SndBuff, 0);
					
				} else {
					UART0->C2 &= ~UART0_C2_TIE_MASK;
				}
			}
		}
		NVIC_ClearPendingIRQ(UART0_IRQn);
	}
