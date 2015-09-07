/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Pro or Pro Mini w/ ATmega328 (5V, 16 MHz), Platform=avr, Package=arduino
*/

#define __AVR_ATmega328P__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
int read_LCD_buttons();
void ZeitUpDown( int lcd_key, uint8_t faktor);
void switchState();
void refreshZeit();
void servoDoEvents();

#include "D:\arduino\arduino-akt\hardware\arduino\avr\variants\standard\pins_arduino.h" 
#include "D:\arduino\arduino-akt\hardware\arduino\avr\cores\arduino\arduino.h"
#include "D:\arduino\arduino-akt\projects\TeaBot\TeaBot.ino"
