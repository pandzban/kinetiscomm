
#ifndef leds_h
#define leds_h

#include "MKL46Z4.h"   /* Device header */
#include "extra.h"

#define ledGreen 0  //Green led is first in led_mask
#define ledRed 1  //Red led is second in led_mask

const static uint32_t led_mask[] = {1UL << 5, 1UL << 29};  //Green led is Port D bit 5, Red led is Port E bit 29

void ledsInitialize(void);

void ledgreenBlink(uint32_t x, uint32_t y);
void ledredBlink(uint32_t x, uint32_t y);
void ledsOff (void);
void ledsOn (void);

#endif
