#ifndef TRANS
#define TRANS

#include "MKL25Z4.h" 
#include "extra.h"
#include "send.h"
#include "leds.h"
#include "assoc.h"


#define MAX_STREAM 258     /// needs to match with PC interfacing
#define G_QUEUE_SIZE 16*MAX_STREAM    /// keep in mind that General Buffer is the biggest buffer in this application, so keep it easy

#define ERROR 99        // these signals must match with definitions in PC and other MCs
#define INIT 107
#define INIT_ACK 117
#define FIN 255
#define FIN_ACK 222
#define DATA_REQ 150



extern volatile uint8_t received;  // important global variables for operations
extern volatile uint8_t received1;
extern volatile uint8_t received2;
extern volatile uint8_t transmit;
extern volatile uint8_t transmit1;
extern volatile uint8_t transmit2;

enum Board_State{wait, gather_info, connected, idle}; // not implemented

extern enum Board_State board_state; // same

enum UART_State{disabled, receive, send};

extern enum UART_State uart0_state;
extern enum UART_State uart1_state;
extern enum UART_State uart2_state;



struct General_Buffer{       // general buffer structure
	uint8_t Data[G_QUEUE_SIZE];
	unsigned int Head;
	unsigned int Tail;
	unsigned int Size;
};

extern struct General_Buffer GenBuff;

enum Arrey_state{none, process, load_empty, loading, ready_to_enq}; // none and process are for Arr type 0, rest for Arr type 1

struct Arrey{
	uint8_t arrey_data[MAX_STREAM];
	unsigned int arrey_element;
	unsigned int total_arrey;
	enum Arrey_state arr_state;
};

extern struct Arrey Arr0, Arr1, Arr2, Arr3;  // 
extern struct Arrey Pit_priv;  // private array

void clear_arrey(struct Arrey * ar, int type);      /// arrays' functions
void arr_get_ready(void);
void arr_append(struct Arrey * arr, struct Arrey * mainarr, uint8_t data, uint8_t force, int total, uint8_t Tag);
void mass_append(struct Arrey * arr, struct Arrey * mainarr, int data, int time, uint8_t force, int total, uint8_t Tag, int Data_size, int Time_size); // to use in other files of devices

enum PC_Trans_State{wait_for_INIT, send_INIT_ACK, send_DATA, send_FIN, Error};  // all posible states of transmission
enum MC_Trans_State{wait_for_INIT2, send_INIT_ACK2, wait_for_DATA_REQ, send_DATA2, wait_for_FIN, send_FIN_ACK, Error3}; 
enum MC_Recv_State{send_INIT, wait_for_INIT_ACK, send_DATA_REQ, wait_for_DATA, send_FIN2, wait_for_FIN_ACK, Error2};

extern enum PC_Trans_State tx_state;      // state object for transmission of stream direction from MC to PC (uart 0)
extern enum MC_Trans_State tx_state2; // for transmission to uart-non-0
extern enum MC_Recv_State rx_state; // for uart 1 receive
extern enum MC_Recv_State rx_state2; // for uart 2  receive

void GB_Init(struct General_Buffer * q);  // functions using General Buffer or connected with it
int GB_is_space(struct General_Buffer * q, int size);
int GB_is_almost_full(struct General_Buffer * q);
int GB_is_empty(struct General_Buffer * q);
int GB_recv_to(struct Arrey * arr, uint8_t word );
int GB_Enqueue(struct General_Buffer * q, struct Arrey * arr);
int GB_Dequeue(struct General_Buffer * q);
void GB_Shift(struct General_Buffer * q);
int GB_to_send( int uart_num );
void Transmit_Data( int uart_num, int repeat);
void Receive_Data( int uart_num, int repeat);
void Receive_Data2(int uart_num, int repeat);
int Load_Data( struct Arrey * arr);

#endif
