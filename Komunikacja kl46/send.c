

#include "send.h"


struct SendQueue SndBuff;//, RcvBuff;

void Q_Initialize(void){
	QS_Init(&SndBuff);
}

void QS_Init(struct SendQueue * q){
	unsigned int i;
	for (i=0; i<SEND_QUEUE_SIZE; i++){
		q->qData[i] = 0; 
	}
		q->qHead = 0;
		q->qTail = 0;
		q->qSize = 0;
}

int QS_Empty(struct SendQueue * q) {
	return q->qSize == 0;
}

int QS_Full(struct SendQueue * q) {
	return q->qSize == SEND_QUEUE_SIZE;
}

int QS_Enqueue(struct SendQueue * q, uint8_t d) {

	if (!QS_Full(q)) {
		q->qData[q->qTail++] = d;
		q->qTail %= SEND_QUEUE_SIZE;
		q->qSize++;
		return 1; // success
	} else
		return 0; // failure
}

uint8_t QS_Dequeue(struct SendQueue * q) {
	uint8_t t = 0;
	if (!QS_Empty(q)) {
		t = q->qData[q->qHead];
		q->qData[q->qHead++] = 0; 
		q->qHead %= SEND_QUEUE_SIZE;
		q->qSize--;
	}
	return t;
}

int QS_Send(struct SendQueue * q, int uart_num){
	int done = 0;
	if (!QS_Empty(q)) {
		switch(uart_num){
			case 0:
				if ((UART0->S1 & UART0_S1_TDRE_MASK)){ // while
					UART0->D = q->qData[q->qHead];
					done = 1;
				}
				break;
			case 1:
				if (UART1->S1 & UART_S1_TDRE_MASK){
					UART1->D = q->qData[q->qHead]; // z tego samego bufra
					done = 1;
				}
				break;
			case 2:
				if (UART2->S1 & UART_S1_TDRE_MASK){
					UART2->D = q->qData[q->qHead];
					done = 1;
				}
				break;
		}
		if (done == 1){
			q->qData[q->qHead++] = 0; 
			q->qHead %= SEND_QUEUE_SIZE;
			q->qSize--;
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int Q_Send(int uart_num, uint8_t word){
	int done = 0;
	switch (uart_num){
		case 0:
			if (UART0->S1 & UART0_S1_TDRE_MASK){
				UART0->D = word;
				done = 1;
			}
			break;
		case 1:
			if (UART1->S1 & UART_S1_TDRE_MASK){
				UART1->D = word;
				done = 1;
			}
			break;
		case 2:
			if (UART2->S1 & UART_S1_TDRE_MASK){
				UART2->D = word;
				done = 1;
			}
			break;
	}
	return done;
}

int Q_Receive(volatile uint8_t * word, int uart_num){
	int done = 0;
	switch(uart_num){
		case 0:
			
			if ((UART0->S1 & UART0_S1_RDRF_MASK)){ // while
				(*word) = UART0->D; 
				done = 1;
			}
			break;
		case 1:
			if (UART1->S1 & UART_S1_RDRF_MASK){
				(*word) = UART1->D; 
				done = 1;
			}
			break;
		case 2:
			if (UART2->S1 & UART_S1_RDRF_MASK){
				(*word) = UART2->D; 
				done = 1;
			}
			break;
	}
	if (done == 1){
		return 1;
	} else {
		return 0;
	}
}
