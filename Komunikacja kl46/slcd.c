

#include "slcd.h"												//Declarations
void slcdDemo(uint16_t);
/*----------------------------------------------------------------------------
  Function that initializes sLCD
 *----------------------------------------------------------------------------*/
void slcdInitialize(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | /* Enable Clock to Port B & C */ 
								SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_SLCD_MASK; /* Enable Clock to Port D & E and sLCD module*/ 
	LCD->GCR |= LCD_GCR_PADSAFE_MASK; // Set PADSAFE during configuration
	LCD->GCR &= ~LCD_GCR_LCDEN_MASK; // Clear LCDEN during configuration
	MCG->C1  |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
	//multiplexers configuration for ports to act as sLCD controller
	
	//TASK 2.3 - replace XXX with pin number, and YYY with proper alternative number
 PORTD->PCR[0] = PORT_PCR_MUX(0); // COM0
 PORTE->PCR[4] = PORT_PCR_MUX(0); // COM1        //// 0 to przepiecie na sLCD
 PORTB->PCR[23] = PORT_PCR_MUX(0); // COM2
 PORTB->PCR[22] = PORT_PCR_MUX(0); // COM3
 PORTC->PCR[17] = PORT_PCR_MUX(0); // SEG0
 PORTB->PCR[21] = PORT_PCR_MUX(0); // SEG1
 PORTB->PCR[7] = PORT_PCR_MUX(0); // SEG2
 PORTB->PCR[8] = PORT_PCR_MUX(0); // SEG3
 PORTE->PCR[5] = PORT_PCR_MUX(0);  // SEG4
 PORTC->PCR[18] = PORT_PCR_MUX(0); // SEG5
 PORTB->PCR[10] = PORT_PCR_MUX(0);  // SEG6
 PORTB->PCR[11] = PORT_PCR_MUX(0); // SEG7
	
	//sLCD register configuration
	LCD->GCR = LCD_GCR_RVTRIM(0x00) | LCD_GCR_CPSEL_MASK | LCD_GCR_LADJ(0x03) |
						 LCD_GCR_VSUPPLY_MASK | LCD_GCR_ALTDIV(0x00) |
						 LCD_GCR_SOURCE_MASK | LCD_GCR_LCLK(0x01) | LCD_GCR_DUTY(0x03);
	//sLCD blinking configuration
	LCD->AR = LCD_AR_BRATE(0x03);	
	//FDCR register configuration
	LCD->FDCR = 0x00000000;
  //activation of 12 pins to control sLCD (2 registers, 32 bits each)
	LCD->PEN[0] = LCD_PEN_PEN(1u<<7) | // LCD_P7
								LCD_PEN_PEN(1u<<8) | // LCD_P8
								LCD_PEN_PEN(1u<<10) | // LCD_P10 
								LCD_PEN_PEN(1u<<11) | // LCD_P11
								LCD_PEN_PEN(1u<<17) | // LCD_P17
								LCD_PEN_PEN(1u<<18) | // LCD_P18
								LCD_PEN_PEN(1u<<19); // LCD_P19
	LCD->PEN[1] = LCD_PEN_PEN(1u<<5) | // LCD_P37
								LCD_PEN_PEN(1u<<6) | // LCD_P38
								LCD_PEN_PEN(1u<<8) | // LCD_P40
								LCD_PEN_PEN(1u<<20) | // LCD_P52
								LCD_PEN_PEN(1u<<21); // LCD_P53
	//configuration of 4 back plane pins (2 registers, 32 bits each)
	LCD->BPEN[0] = LCD_BPEN_BPEN(1u<<18) | //LCD_P18
								 LCD_BPEN_BPEN(1u<<19); //LCD_P19
	LCD->BPEN[1] = LCD_BPEN_BPEN(1u<<8) | //LCD_P40
								 LCD_BPEN_BPEN(1u<<20); //LCD_P52

	// waveform registers configuration – 4 active (because of 4 back planes)
	// (44.3.7 in KL46 Reference Manual)
	LCD->WF[4] =  LCD_WF_WF16(0x00) | LCD_WF_WF17(0x00) |
							  LCD_WF_WF18(0x88) | // COM3 (10001000)
							  LCD_WF_WF19(0x44);  // COM2 (01000100)
	LCD->WF[10] = LCD_WF_WF40(0x11) | // COM0 (00010001)
								LCD_WF_WF41(0x00) | LCD_WF_WF42(0x00) | LCD_WF_WF43(0x00);
	
	LCD->WF[13] = LCD_WF_WF52(0x22) | // COM1 (00100010)
								LCD_WF_WF53(0x00) | LCD_WF_WF54(0x00) | LCD_WF_WF55(0x00);
	
	LCD->GCR &= ~LCD_GCR_PADSAFE_MASK; // Clear PADSAFE at the end of configuration
	LCD->GCR |= LCD_GCR_LCDEN_MASK; // Set LCDEN at the end of configuration
}
/*----------------------------------------------------------------------------
  Function that outputs on sLCD (number) error message for debug purpose
 *----------------------------------------------------------------------------*/
void slcdErr(uint8_t number){
	LCD->WF8B[LCD_Front_Pin[0]] = (LCD_S_G | LCD_S_E | LCD_S_D | LCD_S_F); //1st digit
	LCD->WF8B[LCD_Front_Pin[1]] = (LCD_S_A);
	
	LCD->WF8B[LCD_Front_Pin[2]] = (LCD_S_E | LCD_S_G); //2nd digit
	LCD->WF8B[LCD_Front_Pin[3]] = (LCD_C);
	
	LCD->WF8B[LCD_Front_Pin[4]] = (LCD_S_E | LCD_S_G); //3rd digit
	LCD->WF8B[LCD_Front_Pin[5]] = (LCD_C);
	
	switch(number){  //4 digit depends on provided number
		case 0x00:
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_S_D | LCD_S_E | LCD_S_F); /* To display '0' we must active segments: d, e & f on first front plane */
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_S_A | LCD_S_B | LCD_S_C); /* To display '0' we must active segments: a, b & c on second front plane */
			break;
		case 0x01:
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_C);  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_S_B | LCD_S_C);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x02:
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_S_D | LCD_S_E | LCD_S_G );  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_S_A | LCD_S_B);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x03:
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_S_D | LCD_S_G);  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_S_A | LCD_S_B | LCD_S_C);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x04:
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_S_G | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_S_B | LCD_S_C);
			break;
		default:   //as default display 'r'
			LCD->WF8B[LCD_Front_Pin[6]] = (LCD_S_E | LCD_S_G);
			LCD->WF8B[LCD_Front_Pin[7]] = (LCD_C);
			break;
	}
}
/*----------------------------------------------------------------------------
  Writing on sLCD (value) on specified position (digit)
*
* FP0 - Front Plane 0
*  f |_g
*  e |_
*    d
*
* FP1 - Front Plane 1
*    a_
*      | b
*    c |.
*        dot
 *----------------------------------------------------------------------------*/
void slcdSetTab(uint8_t value, uint8_t digit){ // mniejsza wersja slcdSet()
		if (value > 15 || value < 0){
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = LCD_Set_Tab[0x10]>>2u;
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = LCD_Set_Tab[0x10];
		} else 
		{
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = LCD_Set_Tab[value] >> 8u;
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = LCD_Set_Tab[value];
	}
		slcdDotUpdate(); // sprawdza stan kropek
}

void slcdSet(uint8_t value, uint8_t digit){	

	switch(value){
		case 0x00:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_D | LCD_S_E | LCD_S_F); /* To display '0' we must active segments: d, e & f on first front plane */
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C); /* To display '0' we must active segments: a, b & c on second front plane */
			break;
		case 0x01:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_C);  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_B | LCD_S_C);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x02:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_D | LCD_S_E | LCD_S_G );  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x03:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_D | LCD_S_G);  //TASK 2.5 - replace ZZZ with segment definition
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C);  //TASK 2.5 - replace ZZZ with segment definition
			break;
		case 0x04:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_B | LCD_S_C);
			break;
		case 0x05:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_F | LCD_S_D);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_C);
			break;
		case 0x06:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_E | LCD_S_D  | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_C);
			break;
		case 0x07:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_C);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C);
			break;
		case 0x08:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_E | LCD_S_D  | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C);
			break;
		case 0x09:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_F | LCD_S_G | LCD_S_D);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C);
			break;
		case 0x0A:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_F | LCD_S_E | LCD_S_G);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A | LCD_S_B | LCD_S_C);
			break;
		case 0x0B:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_E | LCD_S_D | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_C);
			break;
		case 0x0C:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_F | LCD_S_E | LCD_S_D);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A);
			break;
		case 0x0D:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_E | LCD_S_D);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_B | LCD_S_C);
			break;
		case 0x0E:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_G | LCD_S_E | LCD_S_D | LCD_S_F);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A);
			break;
		case 0x0F:
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_F | LCD_S_E | LCD_S_G);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_S_A);
			break;
		default:   //as default display 'r'
			LCD->WF8B[LCD_Front_Pin[((2*digit)-2)]] = (LCD_S_E | LCD_S_G);
			LCD->WF8B[LCD_Front_Pin[((2*digit)-1)]] = (LCD_C);
			break;
	}
	if(digit>4){ 
		slcdErr(1);
	}
	slcdDotUpdate(); // sprawdza stan kropek
}
/*----------------------------------------------------------------------------
  Function for conversion purpose, displaying (value) in specified (format)
 *----------------------------------------------------------------------------*/
void slcdDisplay(uint16_t value ,uint16_t format){	
	if (format>16 || format<2 || value<0 || value>(format*format*format*format-1)){
		slcdErr(5);
	}
	else{
	uint16_t tab[4] = {1, 2, 3, 4};
	int i = 0;
	
	/* TASK 2.7: Write value to the table in specified format */
	tab[0] = (value/(format*format*format))%format; // przesuwamy na najnizszy bit i wyciagamy reszte z dzielenia przez dana baze(format)
	tab[1] = (value/(format*format))%format;
	tab[2] = (value/format)%format;
	tab[3] = value%format;
	
	for(i=0; i < 4; i++){		
		slcdSetTab(tab[i], i+1);     ///// uzywa nowej funkcji ustawiajacej bity
	}
}
}
/*----------------------------------------------------------------------------
  Accelerated function for conversion purpose, displaying (value) in specified (format) 
 *----------------------------------------------------------------------------*/
void slcdDisplayTurbo(uint16_t value ,uint16_t format){	 // tylko dla podstaw 2, 4, 8 i 16
	int i = 0;
	uint16_t tab[4] = {1, 2, 3, 4};
	switch(format){
		case 16:
				tab[0] = (value & 0xF000)>>12;
				tab[1] = (value & 0x0F00)>>8;
				tab[2] = (value & 0x00F0)>>4;
				tab[3] = (value & 0x000F)>>0;
				for(i=0; i < 4; i++){		
					slcdSet(tab[i], i+1);
				}
			break;
		case 8:
			if(value > 4095){
				slcdErr(0x01);
			} else{
				tab[0] = (value & 0xE00)>>9;
				tab[1] = (value & 0x1C0)>>6;
				tab[2] = (value & 0x038)>>3;
				tab[3] = (value & 0x007)>>0;
				for(i=0; i < 4; i++){		
					slcdSet(tab[i], i+1);
				}
			}
			break;
		case 4:
			if(value > 255){
				slcdErr(0x01);
			} else{
				tab[0] = (value & 0xC0)>>8;
				tab[1] = (value & 0x30)>>4;
				tab[2] = (value & 0x0C)>>2;
				tab[3] = (value & 0x03)>>0;
				for(i=0; i < 4; i++){		
					slcdSet(tab[i], i+1);
				}
			}
			break;
		case 2:
			if(value > 15){
				slcdErr(0x01);
			} else{
				tab[0] = (value & 8)>>3;
				tab[1] = (value & 4)>>2;
				tab[2] = (value & 2)>>1;
				tab[3] = (value & 1)>>0;
				for(i=0; i < 4; i++){		
					slcdSet(tab[i], i+1);
				}
			}
			break;
		default:
			slcdErr(5);
			break;
	}
}
/*----------------------------------------------------------------------------
  Assert selected Dot in static variable
 *----------------------------------------------------------------------------*/
void slcdDot(uint16_t dot_index){ // 1 - 3 to kropki, 4 to dwukropek
	if (dot_index<5 && dot_index>0){
		if (colon%2 == 0 && dot_index == 1){
			colon += 1;
		//LCD->WF8B[LCD_Front_Pin[((2*dot_index)-1)]] |= (LCD_S_DEC);	
		}
		else if((colon/2)%2 == 0 && dot_index == 2){
			colon += 2;
		}
		else if((colon/4)%2 == 0 && dot_index == 3){
			colon += 4;
		}
		else if((colon/8)%2 == 0 && dot_index == 4){
			colon += 8;
		}
	}
	slcdDotUpdate(); // od razu sprawdza stan kropek
}
/*----------------------------------------------------------------------------
  Clear selected Dot in static variable
 *----------------------------------------------------------------------------*/
void slcdClearDot(uint16_t dot_index){ // 1 - 3 to kropki, 4 to dwukropek
	if (dot_index<5 && dot_index>0){
		if (colon%2 == 1 && dot_index == 1){
			colon -= 1;
		}
		else if((colon/2)%2 == 1 && dot_index == 2){
			colon -= 2;
		}
		else if((colon/4)%2 == 1 && dot_index == 3){
			colon -= 4;
		}
		else if((colon/8)%2 == 1 && dot_index == 4){
			colon -= 8;
		}
	}
	slcdDotUpdate(); // sprawdza stan kropek
}
/*----------------------------------------------------------------------------
  Update current state of Dot setting on display
 *----------------------------------------------------------------------------*/
void slcdDotUpdate(void){
	if (colon != 0){ // colon --> volatile static w slcd.h
		if (colon%2 == 1){
			LCD->WF8B[LCD_Front_Pin[1]] |= (LCD_S_DEC);
		} else {
			LCD->WF8B[LCD_Front_Pin[1]] &= ~(LCD_S_DEC);
		}
		if ((colon/2)%2 == 1){
			LCD->WF8B[LCD_Front_Pin[3]] |= (LCD_S_DEC);
		} else {
			LCD->WF8B[LCD_Front_Pin[3]] &= ~(LCD_S_DEC);
		}
		if ((colon/4)%2 == 1){
			LCD->WF8B[LCD_Front_Pin[5]] |= (LCD_S_DEC);
		} else {
			LCD->WF8B[LCD_Front_Pin[5]] &= ~(LCD_S_DEC);
		}
		if ((colon/8)%2 == 1){
			LCD->WF8B[LCD_Front_Pin[7]] |= (LCD_S_DEC);
		} else {
			LCD->WF8B[LCD_Front_Pin[7]] &= ~(LCD_S_DEC);
		}
	}
}
/*----------------------------------------------------------------------------
  Clear sLCD
 *----------------------------------------------------------------------------*/
void slcdClear(void){
	uint8_t i = 0;
	
	for(i=0; i < 8; i++){
		LCD->WF8B[LCD_Front_Pin[i]] = LCD_C;
	}
}
/*----------------------------------------------------------------------------
  Clear Half of sLCD
 *----------------------------------------------------------------------------*/
void slcdHalfClear(uint8_t side){  // dla side = 0 zostawia prawy nibble, dla innych lewy ( reszte wylacza )
	uint8_t i = 0;
	uint8_t limit1, limit2;
	if (side == 0){
		limit1 = 0;
		limit2 = 4;
	} else {
		limit1 = 4;
		limit2 = 8;
	}
	for(i=limit1; i < limit2; i++){
		LCD->WF8B[LCD_Front_Pin[i]] = LCD_C;
	}
}

/*----------------------------------------------------------------------------
  Function for demo purpose
 *----------------------------------------------------------------------------*/
void slcdDemo(uint16_t format){	
	uint8_t i;
	
	for(i=0; i < 0xFF; i++){
		slcdClear();
		//slcdSet(i, i%4 + 1);
		slcdDisplay(i, format);
		//slcdDisplayTurbo(i, format);
		delay_mc(100);
		slcdClear();
	}
}
/*----------------------------------------------------------------------------
  Function for demo purpose (updates every use)
 *----------------------------------------------------------------------------*/
void slcdDemoStatic(uint16_t format){	
	static volatile uint8_t i = 0;
	if( i < 0xFF){
		i++;
		slcdClear();
		//slcdSet(i, i%4 + 1);
		slcdDisplay(i, format);
		//slcdDisplayTurbo(i, format);
		delay_mc(100);
		slcdClear();
	}
}
