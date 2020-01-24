
#ifndef slcd_h
#define slcd_h

#include "MKL46Z4.h"   /* Device header */
#include "extra.h"

#define LCD_N_FRONT 8
// Definition of segments, each 7-segments (4 digits) is controled by two pins
// Pin 1 -> (2*digit - 1), Pin 2 -> (2*digit - 2)
//  COM  Pin 1 Pin 2
//   0    D    Dec
//   1    E    C
//   2    G    B
//   3    F    A
#define LCD_S_D 0x11 // seg D
#define LCD_S_E 0x22 // seg E
#define LCD_S_G 0x44 // seg G
#define LCD_S_F 0x88 // seg F
#define LCD_S_DEC 0x11
#define LCD_S_C 0x22
#define LCD_S_B 0x44
#define LCD_S_A 0x88
#define LCD_C 0x00 // clear
#define LCD_Half 0x0F 
//definition of each pin connected to sLCD
#define LCD_FRONT0 37u
#define LCD_FRONT1 17u
#define LCD_FRONT2 7u
#define LCD_FRONT3 8u
#define LCD_FRONT4 53u
#define LCD_FRONT5 38u
#define LCD_FRONT6 10u
#define LCD_FRONT7 11u
#define BLINK_A 0u
#define BLINK_B 1u

const static uint16_t LCD_Set_Tab[] = {0xBBEE, 0x0066, 0x77CC, 0x55EE, 0xCC66, 0xDDAA, 0xFFAA, 0x00EE, 0xFFEE, 0xDDEE, 0xEEEE, 0xFF22, 0xDD88, 0x7766, 0xFF88, 0xEE88, 0x6600}; //( slcdSetTab() ) deklaracja tablicy do szybkiego odczytywania do rejestrów waveform
const static uint8_t LCD_Front_Pin[LCD_N_FRONT] = {LCD_FRONT0, LCD_FRONT1, LCD_FRONT2, LCD_FRONT3, LCD_FRONT4, LCD_FRONT5, LCD_FRONT6, LCD_FRONT7};
volatile static uint8_t colon = 0; // 0b0000xxxx -( slcdDot() )  kazdy kolejny bit w xxxx odpowiada kolejno: dwukropkowi, trzem kropkom kolejno: trzeciej , drugiej i pierwszej

void slcdInitialize(void);
void slcdErr(uint8_t number);
void slcdSet(uint8_t value,uint8_t digit);
void slcdSetTab(uint8_t value, uint8_t digit);
void slcdDisplay(uint16_t value, uint16_t format);
void slcdDisplayTurbo(uint16_t value, uint16_t format);
void slcdDot(uint16_t);
void slcdClearDot(uint16_t);
void slcdDotUpdate(void);
void slcdClear(void);
void slcdHalfClear(uint8_t);
void slcdDemo(uint16_t);
void slcdDemoStatic(uint16_t);
	
#endif
