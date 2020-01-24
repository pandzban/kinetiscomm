

#include "assoc.h"

struct Information Confirmed; // holds confirmed sets from PC

struct Information Member; // reference tags and formats for each device /// needs to be filled with functions(not implemented), or manually(append)

uint8_t get_Format(int Data_Size, int Time_Size){
	int time_en = 0;
	int data_s = 0;
	int time_s = 0;
	if (Time_Size > 0){
		time_en = 1;
	}
	if (time_en){
		time_s = Time_Size << 1;
	}
	data_s = Data_Size << 5;
	return (time_s + data_s + time_en);
}

void prepare_Information(void){ // use before any usage of Member object
	Member.No = 0;
	Member.No_checked = 0;
	int u = 0;
	for (u = 0; u < 32; u++){
		Member.Tag[u] = 0;
	}
	for (u = 0; u < 32; u++){
		Member.Format[u] = 0;
	}
}

void prepare_Confirmed(void){  // use before any usage of Confirmed object
	Confirmed.No = 0;
	Confirmed.No_checked = 0;
	int u = 0;
	for (u = 0; u < 32; u++){
		Confirmed.Tag[u] = 0;
	}
	for (u = 0; u < 32; u++){
		Confirmed.Format[u] = 0;
	}
}

void append_Information(uint8_t Tag, uint8_t Format){  // add one set of information
	int Number = Member.No;
	if (Number < 32){
		Member.Tag[Number] = Tag;
		Member.Format[Number] = Format;
		Member.No = Number + 1;
	}
}

void append_Confirmed(uint8_t Tag, uint8_t Format){   // add one set of information
	int Number = Confirmed.No;
	if (Number < 32){
		Confirmed.Tag[Number] = Tag;
		Confirmed.Format[Number] = Format;
		Confirmed.No++;
	}
}

void send_Information(void){        // send all information sets 
	int Number = Member.No;
	uint8_t Tag = 0, Format = 0, Num = 0;
	int u = 0;
	for (u = 0; u < Number ; u++){
		Num = Member.No;
		Tag = Member.Tag[u];
		Format = Member.Format[u];
		while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
		UART0->D = 204;            // important signal for async reader
		while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
		UART0->D = Tag;
		while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
		UART0->D = Number;
		while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
		UART0->D = Format;
	}
}

int check_Information(uint8_t confirm[3]){    // checks if received confirmation sets exists in original
	int k = 0;
	int truth = 0;
		if (Member.No == confirm[1]){
			for (k = 0; k < Member.No; k++){
				if (Member.Tag[k] == confirm[0]){
					if (Member.Format[k] == confirm[2]){
						truth = 1;
					}
				}
			}
		}
	return truth;
}

int check_Confirmed(uint8_t confirm[3]){       // checks if set is already written to Confirmed ( doesn't check No value )
	int k = 0;
	int truth = 0;
		for (k = 0; k < Confirmed.No; k++){
			if (Confirmed.Tag[k] == confirm[0]){
				if (Confirmed.Format[k] == confirm[2]){
					truth = 1;
				}
			}
		}
	return truth;
}

int compare_with_Number(int num){ // compares Numbers // unused
	int number = Member.No;
	if (number == num){
		number = 1;
	}
	else{
		number = 0;
	}
	return number;
}

int compare_Inf_Conf(void){  // chceck if No is the same and all the sets match each other
	int u = 0;
			int k = 0;
	int truth = 0;
	if(Member.No == Confirmed.No){
		for (u = 0; u < Confirmed.No; u++){
			for (k = 0; k < Member.No; k++){
				if(Member.Format[k] == Confirmed.Format[u]){
					if(Member.Tag[k] == Confirmed.Tag[u]){
						truth++;
					}
				}
			}
		}
	}
	if (truth == Member.No){
		return 1;
	} else {
		return 0;
	}
}

int detect_PC(void){
	__disable_irq(); 
	uint8_t leave = 0;
	uint8_t confirm[3];
	prepare_Information();
	prepare_Confirmed();
	int iftrue = 0;
	received = 1;
	do{
		while(!(UART0->S1 & UART0_S1_RDRF_MASK)){}		
		received = UART0->D; 
		delay_mc(5);
	} while( received != 170);//received != 97);
	int m = 0;
	for (m = 0; m < 50; m++){
		while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
		UART0->D = 170;      // mirror engaging signal
	}
	uart0_state = send;
	append_Information(8, 71);       // example fill
	append_Information(40, 71);			 	// example fill
	append_Information(72, 71);
	int u = 0;
			send_Information();
	do {
		iftrue = 0;
		u = 0;
		for (u = 0; u < 3; u++){
			confirm[u] = 0;
		}
		while(!(UART0->S1 & UART0_S1_RDRF_MASK)){}
		confirm[0] = UART0->D;
		while(!(UART0->S1 & UART0_S1_RDRF_MASK)){}
		confirm[1] = UART0->D;
		while(!(UART0->S1 & UART0_S1_RDRF_MASK)){}
		confirm[2] = UART0->D;
			if (confirm[0] == 252){
				//delay_mc(10);
			}
		iftrue = check_Information(confirm); // check if read set exist in Information
		if (iftrue == 1){
			iftrue = check_Confirmed(confirm);  // check if given set exist already in 
			if (iftrue == 0){
				append_Confirmed(confirm[0], confirm[2]); 
			}
		}
		iftrue = compare_Inf_Conf();
		//delay_mc(50);
		if (iftrue == 1){
			leave = 1;       // finish confirmation
		} else if (Confirmed.No >= Member.No){
			prepare_Confirmed();
		}
	} while (leave == 0);	
	int k = 0;
	for (k = 0; k < 3; k++){
			while (!(UART0->S1 & UART0_S1_TDRE_MASK)){}
			UART0->D = 206;  // send before leaving
	}
	board_state = connected;
	if (uart0_state == send || uart0_state == receive){
		NVIC_EnableIRQ(UART0_IRQn);
	}
	NVIC_ClearPendingIRQ(UART0_IRQn);
	__enable_irq();
	return 1;
}

