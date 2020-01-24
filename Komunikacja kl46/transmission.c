/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// Transmission includes all high level functions including buffer managment and arrays interfacing /// you can add your private arrays right here
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "transmission.h"

struct General_Buffer GenBuff;

struct Arrey Arr0, Arr1, Arr2, Arr3;          // global Arrays, Arr0 is send array between GenBuff and SndBuff, Arrn are between Devices and GenBuff (including Arr1 is for UART1-2)
struct Arrey Pit_priv;       // private Arrays used for gathering content inside devices' specialized functions


enum PC_Trans_State tx_state = wait_for_INIT; // initial state
enum MC_Trans_State tx_state2 = wait_for_INIT2;
enum MC_Recv_State rx_state = send_INIT;
enum MC_Recv_State rx_state2 = send_INIT;
enum Board_State board_state = wait;

enum UART_State uart0_state = disabled;
enum UART_State uart1_state = disabled;
enum UART_State uart2_state = disabled;

void GB_Init(struct General_Buffer * q){ // clear buffer allocated memory before start
	int i = 0;
	for (i = 0; i < G_QUEUE_SIZE; i++){
		q->Data[i] = 0;
	}
	q->Head = 0;
	q->Tail = 0;
	q->Size = 0;
}

int GB_is_space(struct General_Buffer * q, int size){ // wheter data fits into buffer
	return ( ( q->Size + size + 2 ) <= G_QUEUE_SIZE);
}

int GB_is_almost_full(struct General_Buffer * q){ // if biggest possible data can be queued
	return( ( q->Size + MAX_STREAM ) > G_QUEUE_SIZE);
}

int GB_should_move(struct General_Buffer * q){
	return ( ( q->Tail + MAX_STREAM > G_QUEUE_SIZE +1) && ( q->Head - MAX_STREAM > MAX_STREAM) );
}

int GB_is_empty(struct General_Buffer * q){
	return ( q->Size == 0 );
}

uint8_t GB_read_first_el(struct General_Buffer * q){
	return (q->Data[q->Head]);
}

int GB_Enqueue(struct General_Buffer * q, struct Arrey * arr){ // enqueue array of data to buffer (preferably global arrays Arr1-x
	uint8_t size = arr->arrey_data[0];
	if (GB_is_space( q, size)){ //////// nw czy to dobrze
		unsigned int i = 0;
		for (i = 0; i < size + 2; i++){
			q->Data[q->Tail + i] = arr->arrey_data[i];			
		}
		q->Size += size + 2;
		q->Tail += size + 2;
		q->Tail %= G_QUEUE_SIZE;
		return 1; // success
	} else {
		return 0;
	}
}

int GB_Dequeue(struct General_Buffer * q){
	uint8_t size;
	if (!(GB_is_empty( q ))){
		size = q->Data[q->Head];
		q->Data[q->Head++] = 0; 
		q->Size--;
		Arr0.arrey_data[0] = size;
		unsigned int i;
		for (i = 0; i < size + 1 ; i++){
			Arr0.arrey_data[1 + i] = q->Data[q->Head];
			q->Data[q->Head++] = 0;
		}
		q->Size -= (size + 1);
		q->Head %= G_QUEUE_SIZE;
		return 1;
	} else {
		return 0;
	}
}

void GB_Shift(struct General_Buffer * q){ // shift to make space
	unsigned int move;
	if (!(GB_is_empty( q ))){
		move = q->Head;
		int i;
		for (i = 0; i < q->Size; i++){
			q->Data[i] = q->Data[i + move];
			q->Data[i + move] = 0; // can be optimalized
		}
		q->Tail -= move;
		q->Head = 0;
	}
}

uint8_t GB_Read( int uart_num ){ // powinienem stworzyc rcvbuf
	uint8_t temp;
	temp = 0;
	switch (uart_num){
		case 0:
			if (UART0->S1 & UART0_S1_RDRF_MASK){
				temp = UART0->D;
				return temp;
			}
			break;
		case 1:
			if (UART1->S1 & UART_S1_RDRF_MASK){
				temp = UART1->D;
				return temp;
			}
			break;
		case 2:
			if (UART2->S1 & UART_S1_RDRF_MASK){
				temp = UART2->D;
				return temp;
			}
			break;
	}
	return ERROR; // error number
}

void clear_arrey(struct Arrey * ar, int type){ // send == 0, load == 1 // function of array
	int i;
	for (i = 0; i < MAX_STREAM; i++){
		ar->arrey_data[i] = 0;
	}
	ar->arrey_element = 0;
	ar->total_arrey = 0;
	if ( type == 0){
		ar->arr_state = none;
	} else if (type == 1){
		ar->arr_state = load_empty;
	}
}

int GB_to_send( int uart_num ){  // using Arr0 global send array, this func appends its elements into send queue serviced by interrupt
	int out = 0;
	if (uart_num == 0){
		if (Arr0.arr_state == none){
			clear_arrey(&Arr0, 0); // clear the extern arre
			Arr0.total_arrey = GB_read_first_el(&GenBuff);
			Arr0.total_arrey += 2;
			if (GB_Dequeue(&GenBuff)==1){	
				if (GB_should_move(&GenBuff)){
					GB_Shift(&GenBuff);
				}
				Arr0.arr_state = process;
				Arr0.arrey_element = 0;
			}
		}
		if (Arr0.arr_state == process){
			if (Arr0.arrey_element < Arr0.total_arrey){
				if (QS_Enqueue(&SndBuff, Arr0.arrey_data[Arr0.arrey_element])){
					Arr0.arrey_element++;
					out = 1;
				}
			}
			if (Arr0.arrey_element == Arr0.total_arrey){
				clear_arrey(&Arr0, 0);
				out = 2; // finished
			}
		}
		return out;
	} else {
		if (Arr0.arr_state == none){
			clear_arrey(&Arr0, 0); // clear the extern arre
			Arr0.total_arrey = GB_read_first_el(&GenBuff);
			Arr0.total_arrey += 2;
			if (GB_Dequeue(&GenBuff)==1){	
				if (GB_should_move(&GenBuff)){
					GB_Shift(&GenBuff);
				}
				Arr0.arr_state = process;
				Arr0.arrey_element = 0;
			}
		}
		if (Arr0.arr_state == process){
			if (Arr0.arrey_element < Arr0.total_arrey){
				if (Q_Send(uart_num, Arr0.arrey_data[Arr0.arrey_element])){//(QS_Enqueue(&SndBuff, Arr0.arrey_data[Arr0.arrey_element])){
					Arr0.arrey_element++;
					out = 1;
				}
			}
			if (Arr0.arrey_element == Arr0.total_arrey){
				clear_arrey(&Arr0, 0);
				out = 2; // finished
			}
		}
		return out;
	}
}

int GB_recv_to(struct Arrey * arr, uint8_t word ){ // load all Arr array and enqueue it to GenBuff
	uint8_t number;
	uint8_t temp = 0;
	int out = 0;
	if (arr->arr_state == load_empty){
		clear_arrey(arr, 1);
		number = word;
		arr->arrey_data[arr->arrey_element++] = number;
		arr->total_arrey = number + 2;
		arr->arr_state = loading;
		out = 1;
	} else if(arr->arr_state == loading){
		arr->arrey_data[arr->arrey_element++] = word;
		out = 1;
	} 
	if (arr->arrey_element == arr->total_arrey){ // finished loading of Arr
		arr->arr_state = ready_to_enq;
		if (Load_Data(arr)){
			out = 2;
		}
	}
	return out;
}

void arr_get_ready(void){   // all global arrays must be included in this fun
	clear_arrey(&Arr0, 0);
	clear_arrey(&Arr1, 1);
	clear_arrey(&Arr2, 1);
	clear_arrey(&Arr3, 1);
}

void arr_append(struct Arrey * arr, struct Arrey * mainarr, uint8_t data, uint8_t force, int total, uint8_t Tag){ // for use in extern files to append from priv private arrays to Arr global scope arrays
	if (total > 256){
		total = 256;
	}
	arr->arr_state = loading;
	arr->total_arrey = total;
	if (!((arr->arrey_element == arr->total_arrey) | (force != 0))){
		arr->arrey_data[arr->arrey_element] = data;
		arr->arrey_element++;
	}
	if ((arr->arrey_element == arr->total_arrey) | (force != 0)){
		if (mainarr->arr_state == load_empty){
		arr->total_arrey = arr->arrey_element;
		int i;
		for(i = 0; i < arr->total_arrey ; i++){
			mainarr->arrey_data[2+i] = arr->arrey_data[i];
		}
		mainarr->arrey_data[0] = arr->total_arrey;
		mainarr->arrey_data[1] = Tag;
		mainarr->arr_state = ready_to_enq;
		clear_arrey(arr,1);
	}
	}
}

void mass_append(struct Arrey * arr, struct Arrey * mainarr, int data, int time, uint8_t force, int total, uint8_t Tag, int Data_size, int Time_size){
	uint8_t Format = 0;
	unsigned int move;
	Format = get_Format(Data_size, Time_size);   // can be used somewhere
	int i = 0;
	for(i = Data_size; i > 0; i--){
		move = 0;
		move = 8*(i-1);
		arr_append(arr, mainarr, data >> move , force, total , Tag);
	}
	for(i = Time_size; i > 0; i--){
		move = 0;
		move = 8*(i-1);
		arr_append(arr, mainarr, time >> move , force, total , Tag);
	}
}

void Transmit_Data(int uart_num, int repeat){ // one use of this function means one write to send queue // not actual writing to uart!!!
	uint8_t temp;
	int i;
	if (uart_num == 0){	
		if (!(GB_is_empty(&GenBuff)) || !(Arr0.arr_state == none) || (tx_state == send_FIN) || (tx_state == Error)){//(!(GB_is_empty(&GenBuff) && Arr0.arr_state == none)){ // checks if buffers are cleared and if there isn't left FIN 
			for (i = 0 ; i < repeat; i++){
				if (tx_state == wait_for_INIT){
					if (received == ERROR){
						tx_state = Error;
					}
					if (received == INIT){
						UART0->C2 &= ~UART0_C2_RIE_MASK;
						tx_state = send_INIT_ACK;
					}
				} else if (tx_state == send_INIT_ACK) {
					delay_mc(1);
					if (QS_Enqueue(&SndBuff,  INIT_ACK) == 1){
					tx_state = send_DATA;
					}
				} else if (tx_state == send_DATA) {
					delay_mc(1);
					if (GB_to_send(0) == 2){
						tx_state = send_FIN;
					}
				} else if (tx_state == send_FIN) {
					delay_mc(1);
					if (QS_Enqueue(&SndBuff,  FIN) == 1){
						tx_state = wait_for_INIT;
						UART0->C2 |= UART0_C2_RIE_MASK;
					}
				} else if (tx_state == Error){
					clear_arrey(&Arr0, 0);
					Q_Initialize();
					tx_state = wait_for_INIT;
				}
				UART0->C2 |= UART0_C2_TIE_MASK;
				if (tx_state == wait_for_INIT){  // quit when whole stream has been sent
					break;
				}
			}
		}
	} else {
		if (!(GB_is_empty(&GenBuff)) || !(Arr0.arr_state == none) || (tx_state2 == wait_for_FIN) || (tx_state2 == send_FIN_ACK) || (tx_state2 == Error3)){
			if ((uart1_state == send && uart_num == 1) || (uart2_state == send && uart_num == 2)){
					for (i = 0 ; i < repeat; i++){
					temp = 0;
					if (tx_state2 == wait_for_INIT2){
						delay_mc(1);
						if (Q_Receive(&transmit1, uart_num)){
							
							if(transmit1 == INIT){    
								tx_state2 = send_INIT_ACK2;
							} else {
								tx_state2 = Error3;
							}
						}
					} else if (tx_state2 == send_INIT_ACK2) {
						delay_mc(1);
						if(Q_Send(uart_num, INIT_ACK)){
							tx_state2 = wait_for_DATA_REQ;
						}
					} else if (tx_state2 == wait_for_DATA_REQ) {
						delay_mc(1);
						if (Q_Receive(&transmit1, uart_num)){
							if(transmit1 == DATA_REQ){    
								tx_state2 = send_DATA2;
							} else {
								tx_state2 = Error3;
							}
						}
					} else if (tx_state2 == send_DATA2) {
						delay_mc(1);
						temp = GB_to_send(uart_num);
						if(temp == 1){
							tx_state2 = wait_for_DATA_REQ;
							num_of_bytes++;
						} else if(temp == 2){
							tx_state2 = wait_for_FIN;
							num_of_bytes++;
						}			
					} else if (tx_state2 == wait_for_FIN) {
						delay_mc(1);
						if (Q_Receive(&transmit1, uart_num)){
							if(transmit1 == FIN){    
								tx_state2 = send_FIN_ACK;
							} else {
								tx_state2 = Error3;
							}
						}
					} else if (tx_state2 == send_FIN_ACK) {
						delay_mc(1);
						if(Q_Send(uart_num, FIN_ACK)){
							tx_state2 = wait_for_INIT2;
						}
					} else if (tx_state2 == Error3){
						clear_arrey(&Arr0, 0);
						tx_state2 = wait_for_INIT2;
					}
					if (tx_state == wait_for_INIT2){  // quit when whole stream has been sent
						break;
					}
				}
			}
		}
	}
}

void Receive_Data(int uart_num, int repeat){
	uint8_t temp;
	int i;
	if ((uart1_state == receive && uart_num == 1)){
		for (i = 0; i < repeat; i++){
			if (rx_state == send_INIT){
				delay_mc(1);
				if(Q_Send(uart_num, INIT)){
					rx_state = wait_for_INIT_ACK;
				}
			} else if (rx_state == wait_for_INIT_ACK){
				delay_mc(1);
				if (Q_Receive(&received1, uart_num)){
					if(received1 == INIT_ACK){ 
						rx_state = send_DATA_REQ;
					} else {
						rx_state = Error2;
					}
				}
				//} else {
				//	rx_state = send_INIT;
				//}
			} else if (rx_state == send_DATA_REQ){
				delay_mc(1);
				if(Q_Send(uart_num, DATA_REQ)){
					rx_state = wait_for_DATA;
				}
			} else if (rx_state == wait_for_DATA){
				delay_mc(1);
				if (Q_Receive(&received1, uart_num)){
					temp = GB_recv_to(&Arr1, received1);
					if (temp == 1){  // kontynuuje wysylanie 256 bajtów
						rx_state = send_DATA_REQ;  // w góre
					} else if(temp == 2) { // wyslano wszystko
						rx_state = send_FIN2;
					} else {
						rx_state = Error2;
					}
				}
			} else if (rx_state == send_FIN2){
				delay_mc(1);
				if(Q_Send(uart_num, FIN)){
					rx_state = wait_for_FIN_ACK;
				}
			} else if (rx_state == wait_for_FIN_ACK){
				delay_mc(1);
				if (Q_Receive(&received1, uart_num)){
					if(received1 == FIN_ACK){    
						rx_state = send_INIT;
						break; // break if whole stream received
					} else {
						rx_state = Error2;
					}
				}
			} else if (rx_state == Error2){
				clear_arrey(&Arr1, 1);
				rx_state = send_INIT;
				received1 = 0;
				break;
			} 
		}
	} else if ((uart2_state == receive && uart_num == 2)){
				for (i = 0; i < repeat; i++){
			if (rx_state2 == send_INIT){
				delay_mc(1);
				if(Q_Send(uart_num, INIT)){
					rx_state2 = wait_for_INIT_ACK;
				}
			} else if (rx_state2 == wait_for_INIT_ACK){
				delay_mc(1);
				if (Q_Receive(&received2, uart_num)){
					if(received2 == INIT_ACK){ 
						rx_state2 = send_DATA_REQ;
					} else {
						rx_state2 = Error2;
					}
				}
				//} else {
				//	rx_state = send_INIT;
				//}
			} else if (rx_state2 == send_DATA_REQ){
				delay_mc(1);
				if(Q_Send(uart_num, DATA_REQ)){
					rx_state2 = wait_for_DATA;
				}
			} else if (rx_state2 == wait_for_DATA){
				delay_mc(1);
				if (Q_Receive(&received2, uart_num)){
					temp = GB_recv_to(&Arr2, received2);
					if (temp == 1){  // kontynuuje wysylanie 256 bajtów
						rx_state2 = send_DATA_REQ;  // w góre
					} else if(temp == 2) { // wyslano wszystko
						rx_state2 = send_FIN2;
					} else {
						rx_state2 = Error2;
					}
				}
			} else if (rx_state2 == send_FIN2){
				delay_mc(1);
				if(Q_Send(uart_num, FIN)){
					rx_state2 = wait_for_FIN_ACK;
				}
			} else if (rx_state2 == wait_for_FIN_ACK){
				delay_mc(1);
				if (Q_Receive(&received2, uart_num)){
					if(received2 == FIN_ACK){ 
						rx_state2 = send_INIT;
						break; // break if whole stream received
					} else {
						rx_state2 = Error2;
					}
				}
			} else if (rx_state2 == Error2){
				clear_arrey(&Arr2, 1);
				rx_state2 = send_INIT;
				received2 = 0;
				break;
			} 
		}
	}
}

int Load_Data( struct Arrey * arr){
	int out = 0;
	if (arr->arr_state == ready_to_enq){
		if (GB_Enqueue(&GenBuff, arr)){
			clear_arrey(arr, 1);
			arr->arr_state = load_empty;
			out = 1;
		}
	}
	return out;
}

