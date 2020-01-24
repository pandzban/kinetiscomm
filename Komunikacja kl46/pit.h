


#include "MKL46Z4.h" 
#include "extra.h"
#include "leds.h"
#include "transmission.h"

volatile extern uint32_t count;
volatile extern uint8_t display;
volatile extern uint8_t calculate;
volatile extern uint32_t num_of_bytes;
volatile extern uint32_t num_of_bytes4;
void pitInitialize(void);
void systickInit(void);
uint32_t get_pit_time(void);

