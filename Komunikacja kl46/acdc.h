#ifndef ACDC
#define ACDC
#include "MKL46Z4.h"   
#include "pit.h"
#include "transmission.h"

volatile extern uint8_t calculate;

void acdcint(void);
uint16_t swiatlo(void);
uint16_t get_acdc(void);

#endif
