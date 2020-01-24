
#ifndef buttons_h
#define buttons_h

#include "MKL46Z4.h"   /* Device header */

#define button1 0												/* Button 1 is first in button_mask*/
#define button3 1												/* Button 3 is second in button_mask */

const static uint32_t button_mask[] = {1UL << 3, 1UL << 12};  //Button 1 is Port C bit 3, button 3 is Port C bit 12

void buttonsInitialize(void);
int32_t button1Read(void);
int32_t button3Read(void);

#endif
