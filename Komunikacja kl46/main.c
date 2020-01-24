/*
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
				Microprocessor Technology Project, Made by Adam Gawlik and Christopher Dudzik
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
*/

#include "MKL46Z4.h"                    	/* Device header */
#include "leds.h"													/* Leds functions for FRDM46-KL46 */
#include "slcd.h"											    /* Segment LCD of FRDM46-KL46 */
#include "extra.h"	
#include "acdc.h"
#include "pit.h"
#include "tpm.h"
#include "buttons.h"
#include "uart.h"
#include "assoc.h"
#include "transmission.h"
#include "send.h"

#define BOARD_NUMBER 2    // not implemented

 ///  you need to write devices Tag manually in each service function /// in this example case it's acdc.c file TAG constant

volatile uint32_t czas = 0;
volatile uint32_t count = 0;
volatile uint8_t wartosc = 0;
volatile uint8_t counting = 0;
volatile uint8_t display = 0;
volatile uint8_t received = 0;
volatile uint8_t received1 = 0;
volatile uint8_t received2 = 0;
volatile uint8_t transmit = 0;
volatile uint8_t transmit1 = 0;
volatile uint8_t transmit2 = 0;
volatile uint32_t num_of_bytes = 0;
volatile uint32_t num_of_bytes4 = 0;

	void Initialization_List(void){
	slcdInitialize();
	ledsInitialize();
	//tpmInitialize();
	pitInitialize();
	systickInit();
	acdcint();
	arr_get_ready();
	Q_Initialize();
	GB_Init(&GenBuff);
	uart0Initialize();
	uart12Initialize();
	slcdDisplay(0,10);
}

void Set_Uarts_Functions(void){      // set direction of stream 
	uart0_state = disabled;
	uart1_state = send;
	uart2_state = receive;
}

void Critical_Section(void);
void Receive(int uart1_repeats, int uart2_repeats);
void Transmit(int uart_repeats);
void Load_Arrays( void );
	
	int main(void){
	
	Set_Uarts_Functions();
	Initialization_List();
	//detect_PC();   // in this example this board is not being connected to PC, so uart0 blocking is disabled
	
	while(1){
		
		Critical_Section();
		Receive(3, 7);  // these  values can be manipulated for better performance
		Load_Arrays();
		Transmit(3);  // ^^^
		display_slcd();
		
	}
}

void Critical_Section(void){ // routines that requires more calculations
	if (tx_state == wait_for_INIT){
		get_acdc();
	}
}

void Receive( int uart1_repeats, int uart2_repeats){
	if (uart1_state == receive){
		Receive_Data(1, uart1_repeats);
	}
	if (uart2_state == receive){
		Receive_Data(2, uart2_repeats);
	}
}

void Transmit(int uart_repeats){
	if (uart0_state == send){
		Transmit_Data(0, uart_repeats);
	} else if(uart1_state == send){
		Transmit_Data(1, uart_repeats);
	} else if(uart2_state == send){
		Transmit_Data(2, uart_repeats);
	}
}

void Load_Arrays( void ){
	//Load_Data( &Arr1); // functionally loaded into GenBuff
	//Load_Data( &Arr2); // functionally loaded into GenBuff
	Load_Data( &Arr3 );
}


