//#if ARDUINO >= 100

//GIMSK       --> EIMSK

// OW_PORT Pin 4  - PD2
// Analog input PB2

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

//Times
#define OWT_MIN_RESET 45     //minimum duration of the Reset impulse
#define OWT_RESET_PRESENCE 4 //time between rising edge of reset impulse and presence
#define OWT_PRESENCE 20      //duration of the presence impulse
#define OWT_READLINE 3       //for fast master, 4 for slow master and long lines
#define OWT_LOWTIME 3        //for fast master, 4 for slow master and long lines 

//Initializations of AVR
#define INIT_AVR CLKPR=(1<<CLKPCE); \
				   CLKPR=0; /*8Mhz*/  \
				   TIMSK0=0; \
				   EIMSK=(1<<INT0);  /*set direct GIMSK register*/ \
				   TCCR0B=(1<<CS00)|(1<<CS01); /*8mhz /64 couse 8 bit Timer interrupt every 8us*/
				   
#ifdef DS2423
//Setup Counter
#define PC_INT_ISR ISR(PCINT_vect) { /*ATT2313 without 0 by PCINT*/ \
					if (((PINB&(1<<PINB3))==0)&&((istat&(1<<PINB3))==(1<<PINB3))) {	Counter0++;	}		\
					if (((PINB&(1<<PINB4))==0)&&((istat&(1<<PINB4))==(1<<PINB4))) {	Counter1++;	}		\
					istat=PINB;}	\

#define INIT_COUNTER_PINS /* Counter Interrupt */\
						GIMSK|=(1<<PCIE);\
						PCMSK=(1<<PCINT3)|(1<<PCINT4);	\
						DDRB &=~((1<<PINB3)|(1<<PINB4)); \
						istat=PINB;\
#endif

#ifdef DS18B20
//Setup Temp Measurement DS18B20 intern sensor
#define INIT_TEMP  ADMUX=(1<<REFS1)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0); \
					ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); /*ADC Freq: ~63khz*/ \
					ADCSRB=0;
#define CONV_TEMP	{ uint8_t tc; int16_t sum=0; \
						for(tc=0;tc<0x10;tc++) { \
							ADCSRA|=(1<<ADSC)|(1<<ADIF);\
							while(ADCSRA&(1<<ADSC));\
							sum=sum+ADC; \
						} \
						sum=sum-0xDAF;/*calibration  0x010 are 1 K*/\
						/*sum=(sum<<4);*/  \
						scratchpad[0]=0x00ff&sum;\
						scratchpad[1]=0x00ff&(sum>>8);\
						}			
#endif

//#endif // ARDUINO 
