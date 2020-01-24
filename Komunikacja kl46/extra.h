

#ifndef extra_h
#define extra_h

#include "slcd.h"

#include "MKL46Z4.h"   /* Device header */	

volatile extern uint8_t display;
volatile extern uint8_t wartosc;
volatile extern uint32_t num_of_bytes4;
void display_slcd(void);
void delay_mc(uint32_t value);

#endif
