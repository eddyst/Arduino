//owdevice - A small 1-Wire emulator for AVR Microcontroller
//
//Copyright (C) 2012  Tobias Mueller mail (at) tobynet.de
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
// any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
//VERSION 1.1 1Wire Barometric Pressure Sensor ATMEGA48 


#define F_CPU 8000000UL

#include "i2c.h"
#include "bmp085.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>




#ifdef __AVR_ATmega48__ 

// OW_PORT Pin 4  - PD2


//OW Pin
#define OW_PORT PORTD //1 Wire Port
#define OW_PIN PIND //1 Wire Pin as number
#define OW_PORTN (1<<PIND2)  //Pin as bit in registers
#define OW_PINN (1<<PIND2)
#define OW_DDR DDRD  //pin direction register
#define SET_LOW OW_DDR|=OW_PINN;OW_PORT&=~OW_PORTN;  //set 1-Wire line to low
#define RESET_LOW {OW_DDR&=~OW_PINN;}  //set 1-Wire pin as input
//Pin interrupt	
#define EN_OWINT {EIMSK|=(1<<INT0);EIFR|=(1<<INTF0);}  //enable interrupt 
#define DIS_OWINT  EIMSK&=~(1<<INT0);  //disable interrupt
#define SET_RISING EICRA=(1<<ISC01)|(1<<ISC00);  //set interrupt at rising edge
#define SET_FALLING EICRA=(1<<ISC01); //set interrupt at falling edge
#define CHK_INT_EN (EIMSK&(1<<INT0))==(1<<INT0) //test if interrupt enabled
#define PIN_INT ISR(INT0_vect)  // the interrupt service routine
//Timer Interrupt
#define EN_TIMER {TIMSK0 |= (1<<TOIE0); TIFR0|=(1<<TOV0);} //enable timer interrupt
#define DIS_TIMER TIMSK0 &=~(1<<TOIE0); // disable timer interrupt
#define TCNT_REG TCNT0  //register of timer-counter
#define TIMER_INT ISR(TIMER0_OVF_vect) //the timer interrupt service routine


#define OWT_MIN_RESET 45
#define OWT_RESET_PRESENCE 4
#define OWT_PRESENCE 20 
#define OWT_READLINE 3 //for fast master, 4 for slow master and long lines
#define OWT_LOWTIME 3 //for fast master, 4 for slow master and long lines 

//Initializations of AVR
#define INIT_AVR CLKPR=(1<<CLKPCE); \
				   CLKPR=0; /*8Mhz*/  \
				   TIMSK0=0; \
				   EIMSK=(1<<INT0);  /*set direct GIMSK register*/ \
				   TCCR0B=(1<<CS00)|(1<<CS01); /*8mhz /64 couse 8 bit Timer interrupt every 8us*/
				   
				   

		


#endif // __AVR_ATtiny48__ 


//Bits in State
//Bit 0 : 0 no oversampling (4.5 ms), 1 oversampling (25.5 ms)
//Bit 1 : 0 one measurement, 1 16 measurements
//Bit 2 : 1 continuously measurement every (3*((Bit 5-7)+1)>>4) min
//Bit 4 : 1 measurement in progress 
//Bit 5-7: Measurement pause 00001xxx -> 1 minuten 11111xxx 16 minuten

#define MS_OVERAMPLING 1
#define MS_MULTIMESS 2
#define MS_CONTINUOSLY 4
#define MS_PROGRESS 8



typedef union {
    volatile uint8_t bytes[10];
    struct {
		 uint8_t state;
         long temperature;
	      long pressure;
	      uint8_t crc;
	    };
    struct {
		 uint8_t state1;  //Values written in state
         short s1;
         short s2;
         short s3;
         short s4;
         uint8_t crc1;   //Values written in crc
	    };
	} scratchpad_t;
scratchpad_t scratchpad;

volatile long temperature;
volatile long pressure;

volatile uint8_t scrc; //CRC calculation

volatile uint8_t cbuf; //Input buffer for a command
const uint8_t owid[8]={0xA2, 0xA2, 0x10, 0x84, 0x00, 0x00, 0x01, 0xC5};//{0x28, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x02, 0xEA};  
	
//set your own ID http://www.tm3d.de/index.php/tools/14-crc8-berechnung
volatile uint8_t bitp;  //pointer to current Byte
volatile uint8_t bytep; //pointer to current Bit

volatile uint8_t mode; //state
volatile uint8_t wmode; //if 0 next bit that send the device is  0
volatile uint8_t actbit; //current
volatile uint8_t srcount; //counter for search rom

//States / Modes
#define OWM_SLEEP 0  //Waiting for next reset pulse
#define OWM_RESET 1  //Reset pulse received 
#define OWM_PRESENCE 2  //sending presence pulse
#define OWM_READ_COMMAND 3 //read 8 bit of command
#define OWM_SEARCH_ROM 4  //SEARCH_ROM algorithms
#define OWM_MATCH_ROM 5  //test number
#define OWM_READ_SCRATCHPAD 6   
#define OWM_WRITE_SCRATCHPAD 7
#define OWM_CHK_RESET 8  //waiting of rising edge from reset pulse

//Write a bit after next falling edge from master
//its for sending a zero as soon as possible 
#define OWW_NO_WRITE 2
#define OWW_WRITE_1 1
#define OWW_WRITE_0 0




PIN_INT {
	uint8_t lwmode=wmode;  //let this variables in registers
	uint8_t lmode=mode;
	if ((lwmode==OWW_WRITE_0)) {SET_LOW;lwmode=OWW_NO_WRITE;}    //if necessary set 0-Bit 
	DIS_OWINT; //disable interrupt, only in OWM_SLEEP mode it is active
	switch (lmode) {
		case OWM_SLEEP:  
			TCNT_REG=~(OWT_MIN_RESET);
			EN_OWINT; //other edges ?
			break;
		//start of reading with falling edge from master, reading closed in timer isr
		case OWM_MATCH_ROM:  //falling edge wait for receive 
		case OWM_WRITE_SCRATCHPAD:
		case OWM_READ_COMMAND:
			TCNT_REG=~(OWT_READLINE); //wait a time for reading
			break;
		case OWM_SEARCH_ROM:   //Search algorithm waiting for receive or send
			if (srcount<2) { //this means bit or complement is writing, 
				TCNT_REG=~(OWT_LOWTIME);
			} else 
				TCNT_REG=~(OWT_READLINE);  //init for read answer of master 
			break;
		case OWM_READ_SCRATCHPAD:  //a bit is sending 
			TCNT_REG=~(OWT_LOWTIME);
			break;
		case OWM_CHK_RESET:  //rising edge of reset pulse
			SET_FALLING; 
			TCNT_REG=~(OWT_RESET_PRESENCE);  //waiting for sending presence pulse
			lmode=OWM_RESET;
			break;
	}
	EN_TIMER;
	mode=lmode;
	wmode=lwmode;
	
}			

	

TIMER_INT {
	uint8_t lwmode=wmode; //let this variables in registers
	uint8_t lmode=mode;
	uint8_t lbytep=bytep;
	uint8_t lbitp=bitp;
	uint8_t lsrcount=srcount;
	uint8_t lactbit=actbit;
	uint8_t lscrc=scrc;
	//Ask input line sate 
	uint8_t p=((OW_PIN&OW_PINN)==OW_PINN);  
	//Interrupt still active ?
	if (CHK_INT_EN) {
		//maybe reset pulse
		if (p==0) { 
			lmode=OWM_CHK_RESET;  //wait for rising edge
			SET_RISING; 
		}
		DIS_TIMER;
	} else
	switch (lmode) {
		case OWM_RESET:  //Reset pulse and time after is finished, now go in presence state
			lmode=OWM_PRESENCE;
			SET_LOW;
			TCNT_REG=~(OWT_PRESENCE);
			DIS_OWINT;  //No Pin interrupt necessary only wait for presence is done
			break;
		case OWM_PRESENCE:
			RESET_LOW;  //Presence is done now wait for a command
			lmode=OWM_READ_COMMAND;
			cbuf=0;lbitp=1;  //Command buffer have to set zero, only set bits will write in
			break;
		case OWM_READ_COMMAND:
			if (p) {  //Set bit if line high 
				cbuf|=lbitp;
			} 
			lbitp=(lbitp<<1);
			if (!lbitp) { //8-Bits read
				lbitp=1;
				switch (cbuf) {
					case 0x55:lbytep=0;lmode=OWM_MATCH_ROM;break;
					case 0xF0:  //initialize search rom
						lmode=OWM_SEARCH_ROM;
						lsrcount=0;
						lbytep=0;
						lactbit=(owid[lbytep]&lbitp)==lbitp; //set actual bit
						lwmode=lactbit;  //prepare for writing when next falling edge
						break;
					case 0x4E:
						lmode=OWM_WRITE_SCRATCHPAD; 
						lbytep=0;scratchpad.bytes[0]=0;  //initialize writing position in scratch pad 
						break;
					case 0x44:  //Start Convert 
					case 0x64:  // some tool uses this command
						if ((scratchpad.state&MS_CONTINUOSLY) ==0) {
							scratchpad.state|=MS_PROGRESS;
						}
						lmode=OWM_SLEEP;
						break;
					case 0x47:  //Copy Calibration1 to scratchpad
					case 0x67:  
						if ((scratchpad.state&MS_CONTINUOSLY) ==0) {
							scratchpad.s1=calibr.ac1;
							scratchpad.s2=calibr.ac2;
							scratchpad.s3=calibr.ac3;
							scratchpad.s4=calibr.ac4;
						}
						lmode=OWM_SLEEP;
						break;
					case 0x48:  //Copy Calibration2 to scratchpad
					case 0x68:  
						if ((scratchpad.state&MS_CONTINUOSLY) ==0) {
							scratchpad.s1=calibr.ac5;
							scratchpad.s2=calibr.ac6;
							scratchpad.s3=calibr.b1;
							scratchpad.s4=calibr.b2;
						}
						lmode=OWM_SLEEP;
						break;
					case 0x49:  //Copy Calibration2 to scratchpad
					case 0x69:  
						if ((scratchpad.state&MS_CONTINUOSLY) ==0) {
							scratchpad.s1=calibr.mb;
							scratchpad.s2=calibr.mc;
							scratchpad.s3=calibr.md;
							scratchpad.s4=0;
						}
						lmode=OWM_SLEEP;
						break;
					case 0xBE:
						lmode=OWM_READ_SCRATCHPAD; //read scratch pad 
						lbytep=0;lscrc=0; //from first position
						lactbit=(lbitp&scratchpad.bytes[0])==lbitp;
						lwmode=lactbit; //prepare for send firs bit 
						break;
					default: lmode=OWM_SLEEP;  //all other commands do nothing
				}		
			}			
			break;
		case OWM_SEARCH_ROM:
			RESET_LOW;  //Set low also if nothing send (branch takes time and memory)
			lsrcount++;  //next search rom mode
			switch (lsrcount) {
				case 1:lwmode=!lactbit;  //preparation sending complement
					break;
				case 3:
					if (p!=(lactbit==1)) {  //check master bit
						lmode=OWM_SLEEP;  //not the same go sleep
					} else {
						lbitp=(lbitp<<1);  //prepare next bit
						if (lbitp==0) {
							lbitp=1;
							lbytep++;
							if (lbytep>=8) {
								lmode=OWM_SLEEP;  //all bits processed 
								break;
							}
						}				
						lsrcount=0;
						lactbit=(owid[lbytep]&lbitp)==lbitp;
						lwmode=lactbit;
					}		
					break;			
			}
			break;
		case OWM_MATCH_ROM:
			if (p==((owid[lbytep]&lbitp)==lbitp)) {  //Compare with ID Buffer
				lbitp=(lbitp<<1);
				if (!lbitp) {
					lbytep++;
					lbitp=1;
					if (lbytep>=8) {
						lmode=OWM_READ_COMMAND;  //same? get next command
						
						cbuf=0;
						break;			
					}
				} 
			} else {
				lmode=OWM_SLEEP;
			}
			break;
		case OWM_WRITE_SCRATCHPAD:
			if (p) {
				scratchpad.bytes[lbytep]|=lbitp;
			} 
			lbitp=(lbitp<<1);
			if (!lbitp) {		
				lbytep++;
				lbitp=1;
				if (lbytep==1) {  //only status byte can be written
					if ((scratchpad.state&MS_CONTINUOSLY) !=0) { //start Continuous mode get first values
						scratchpad.state|=MS_PROGRESS;
					}
					lmode=OWM_SLEEP;
					break;
				} else scratchpad.bytes[lbytep]=0;
			}		
			break;	
		case OWM_READ_SCRATCHPAD:
			RESET_LOW;
			if ((lscrc&1)!=lactbit) lscrc=(lscrc>>1)^0x8c; else lscrc >>=1;
			lbitp=(lbitp<<1);
			if (!lbitp) {		
				lbytep++;
				lbitp=1;
				if (lbytep>=10) {
					lmode=OWM_SLEEP;
					break;			
				} else if (lbytep==9) scratchpad.bytes[9]=lscrc;
			}					
			lactbit=(lbitp&scratchpad.bytes[lbytep])==lbitp;
			lwmode=lactbit;
			break;
		}
		if (lmode==OWM_SLEEP) {DIS_TIMER;}
		if (lmode!=OWM_PRESENCE)  { 
			TCNT_REG=~(OWT_MIN_RESET-OWT_READLINE);  //OWT_READLINE around OWT_LOWTIME
			EN_OWINT;
		}
		
		mode=lmode;
		wmode=lwmode;
		bytep=lbytep;
		bitp=lbitp;
		srcount=lsrcount;
		actbit=lactbit;
		scrc=lscrc;
}



int main(void)
{	
	//long altitude = 0;
	//double temp = 0;
	
	ioinit();
	i2cInit();
	_delay_ms(100);

	mode=OWM_SLEEP;
	wmode=OWW_NO_WRITE;
	OW_DDR&=~OW_PINN;
	
	SET_FALLING;
	
	INIT_AVR
	
	DIS_TIMER;
	
	BMP085_Calibration();

	scratchpad.state=0;

	sei();


	

	uint32_t counter =0;
	
	while(1) {
		int i,l;
		if (counter==0) {
			counter=5900*((scratchpad.state>>4)+1);
			if ((scratchpad.state&MS_CONTINUOSLY) !=0) {
				scratchpad.state|=MS_PROGRESS;
			}
		}
		//scratchpad.state&=~MS_MULTIMESS; //Testing
		if ((scratchpad.state&MS_PROGRESS)!=0) {
			//BMP085_Calibration();
			if ((scratchpad.state&MS_MULTIMESS)!=0) l=16; else l=1;
			long temp;
			long press;
			temperature=0;
			pressure=0;
			for(i=0;i<l;i++) {
				bmp085Convert(&temp,&press,(scratchpad.state&MS_OVERAMPLING)?3:0);
				temperature+=temp;
				pressure+=press;
			}
			if ((scratchpad.state&MS_MULTIMESS)!=0) {
				temperature=temperature>>4;
				pressure=pressure>>4;
			}
			scratchpad.temperature=temperature;
			scratchpad.pressure=pressure;
			scratchpad.state&=~MS_PROGRESS;
		}
		counter--;
		_delay_ms(10);
	}
}


