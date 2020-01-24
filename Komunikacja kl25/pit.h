
#include "MKL25Z4.h" 
#include "extra.h"
#include "leds.h"
#include "transmission.h"

volatile extern uint32_t count;
volatile extern uint8_t display;
volatile extern uint8_t calculate;

void pitInitialize(void);
void systickInit(void);
uint32_t get_pit_time(void);

