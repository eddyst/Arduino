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
//VERSION 1.2 INTERNTEMP  ATTINY13 (AD input) and ATTINY25 (internal sensor)
//  AND
//VERSION 1.2 DS2423  for ATTINY2313 and ATTINY25
//  AND
//VERSION 1.1 1Wire Barometric Pressure Sensor ATMEGA48 

#define INTERNTEMP //INTERNTEMP ODER DS2423 ODER Barometer ODER WMZ

#ifdef Barometer
#define F_CPU 8000000UL

#include "i2c.h"
#include "bmp085.h"
#include <util/delay.h>
#endif

#include <avr/io.h>
#include <avr/interrupt.h>


//GLobal defines of OWS States / Modes
#define OWM_SLEEP 0  //Waiting for next reset pulse
#define OWM_RESET 1  //Reset pulse received 
#define OWM_PRESENCE 2  //sending presence pulse
#define OWM_READ_COMMAND 3 //read 8 bit of command
#define OWM_SEARCH_ROM 4  //SEARCH_ROM algorithms
#define OWM_MATCH_ROM 5  //test number
#define OWM_GET_ADRESS 6
#define OWM_READ_MEMORY_COUNTER 7
#define OWM_CHK_RESET 8  //waiting of rising edge from reset pulse
#define OWM_WRITE_SCRATCHPAD 9
#define OWM_READ_SCRATCHPAD 10
#define OWM_READ_ROM 50


#ifdef INTERNTEMP
const uint8_t owid[8]={ 0x28, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x02, 0xEA};  
#endif

#ifdef DS2423
const uint8_t owid[8]={ 0x1D, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x02, 0x37};    
#endif

#ifdef Barometer
const uint8_t owid[8]={ 0xA2, 0xA2, 0x10, 0x84, 0x00, 0x00, 0x01, 0xC5};
#endif
//set your own ID http://www.tm3d.de/index.php/tools/14-crc8-berechnung

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
#define EN_TIMER {TIMSK2 |= (1<<TOIE0); TIFR0|=(1<<TOV0);} //enable timer interrupt
#define DIS_TIMER TIMSK2 &=~(1<<TOIE0); // disable timer interrupt
#define TCNT_REG TCNT0  //register of timer-counter
#define TIMER_INT ISR(TIMER2_OVF_vect) //the timer interrupt service routine

//Times
#define OWT_MIN_RESET 45     //minimum duration of the Reset impulse
#define OWT_RESET_PRESENCE 4 //time between rising edge of reset impulse and presence
#define OWT_PRESENCE 20      //duration of the presence impulse
#define OWT_READLINE 3       //for fast master, 4 for slow master and long lines
#define OWT_LOWTIME 3        //for fast master, 4 for slow master and long lines 

#ifdef DS2423
typedef union {
  volatile uint8_t bytes[13];//={1,1,2,0,0,0,0,0,0,0,0,5,5};
  struct {
    uint16_t addr;
    uint8_t read;
    uint32_t counter;
    uint32_t zero;
    uint16_t crc;
  };
} 
scratchpad_t;
scratchpad_t scratchpad;

volatile uint8_t lastcps;
volatile uint32_t Counter0;
volatile uint32_t Counter1;
volatile uint8_t istat;
#endif

#ifdef INTERNTEMP
//volatile uint8_t scratchpad[9]={ 0x50,0x05,0x0,0x0,0x7f,0xff,0x00,0x10,0x0}; //Initial scratchpad
  typedef union {
  volatile uint8_t bytes[9];
  struct {
    uint8_t temperatureLByte;
    uint8_t temperatureHByte;
    uint8_t s1, s2, s3, s4, s5, s6;
    uint8_t crc;
  };
} 
scratchpad_t;
scratchpad_t scratchpad;
#endif

#ifdef Barometer
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
  /*
  struct {
    uint8_t state1;  //Values written in state
    short s1;
    short s2;
    short s3;
    short s4;
    uint8_t crc1;   //Values written in crc
  };*/
} 
scratchpad_t;
scratchpad_t scratchpad;

volatile long temperature;
volatile long pressure;
#endif

#ifdef DS2423
volatile uint16_t scrc; //CRC calculation
#else
volatile uint8_t scrc; //CRC calculation
#endif




#ifdef DS2423

ISR(PCINT_vect) {  //for counting  defined for specific device
  if (((PINB&(1<<PINB3))==0)&&((istat&(1<<PINB3))==(1<<PINB3))) {	
    Counter0++;	
  }		
  if (((PINB&(1<<PINB4))==0)&&((istat&(1<<PINB4))==(1<<PINB4))) {	
    Counter1++;	
  }		
  istat=PINB;
} 
#endif
  
void setup() {
  OWSInit();
#ifdef INTERNTEMP
 //Initial scratchpad
  scratchpad.bytes[0] = 0x50;
  scratchpad.bytes[1] = 0x05;
  scratchpad.bytes[2] = 0x0;
  scratchpad.bytes[3] = 0x0;
  scratchpad.bytes[4] = 0x7f;
  scratchpad.bytes[5] = 0xff;
  scratchpad.bytes[6] = 0x00;
  scratchpad.bytes[7] = 0x10;
  scratchpad.bytes[8] = 0x0;
  //Setup Temp Measurement INTERNTEMP intern sensor
  ADMUX=(1<<REFS1)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); 
  ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC Freq: ~63khz 
  ADCSRB=0;
#endif

#ifdef DS2423
  uint8_t i;
  for(i=0;i<sizeof(scratchpad);i++) scratchpad.bytes[i]=0;
  Counter0=0;
  Counter1=0;
  //Setup Counter Interrupt
  EIMSK|=(1<<PCIE0);
  PCMSK=(1<<PCINT3)|(1<<PCINT4);	
  DDRB &=~((1<<PINB3)|(1<<PINB4)); 
  istat=PINB;
#endif

#ifdef Barometer
  //long altitude = 0;
  //double temp = 0;

  ioinit();
  i2cInit();
  _delay_ms(100);
  BMP085_Calibration();
  scratchpad.state=0;
  uint32_t counter =0;
#endif	
  sei();
}

void loop() {
#ifdef Barometer
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
    if ((scratchpad.state&MS_MULTIMESS)!=0) l=16; 
    else l=1;
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
#endif	
}	

void OWS_CMD( uint8_t cbuf, uint8_t &lmode, uint8_t &bytes) {
  switch (cbuf) {
        case 0x4E:
          lmode=OWM_WRITE_SCRATCHPAD;
#ifdef Barometer
          bytes = 0;
#endif
#ifdef INTERNTEMP
          bytes = 2;
#endif
          //lbytep=2; //ToDo: Das soll ein dummer Puffer werden der immer ab 0 beschrieben wird. OWS_CMD_DONE kann den dann auswerten
          scratchpad.bytes[0]=0;  //initialize writing position in scratch pad 
          break;
        case 0x44:  //Start Convert 
        case 0x64:  // some tool uses this command
#ifdef INTERNTEMP
          //Setup Temp Measurement INTERNTEMP intern sensor
          { 
            uint8_t tc; 
            int16_t sum=0; 
            for(tc=0;tc<0x10;tc++) { 
              ADCSRA|=(1<<ADSC)|(1<<ADIF);
              while(ADCSRA&(1<<ADSC));
              sum=sum+ADC; 
            } 
            sum=sum-0xDAF;//calibration  0x010 are 1 K
            //sum=(sum<<4);
            scratchpad.bytes[0]=0x00ff & sum;
            scratchpad.bytes[1]=0x00ff & (sum>>8);
          }			
#endif
#ifdef Barometer
          if ((scratchpad.state&MS_CONTINUOSLY) ==0) {
            scratchpad.state|=MS_PROGRESS;
          }
#endif
          lmode=OWM_SLEEP;
          break;
#ifdef Barometer
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
#endif
#if defined(Barometer) || defined(INTERNTEMP)
        case 0xBE:
          lmode=OWM_READ_SCRATCHPAD; //read scratch pad 
          lbytep=0;
          lscrc=0; //from first position
          lactbit=(lbitp&scratchpad.bytes[0])==lbitp;
          lwmode=lactbit; //prepare for send firs bit 
          break;
#endif
#ifdef DS2423
        case 0xA5:
          lmode=OWM_GET_ADRESS; //first the master send an address 
          lbytep=0;
          lscrc=0x7bc0; //CRC16 of 0xA5
          scratchpad.bytes[0]=0;
          break;
#endif
}
}

