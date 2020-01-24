

#include "MKL46Z4.h" 
#include "extra.h"
#include "transmission.h"

#ifndef ASSOC
#define ASSOC
struct Information{
	uint8_t No;   // number of devices signed in
	uint8_t Tag[32];
	uint8_t Format[32];
	unsigned int No_checked;
};

extern struct Information Member; // holds authentic Information sets about devices
extern volatile uint8_t received;

uint8_t get_Format(int Data_Size, int Time_Size);
int detect_PC(void);
#endif
