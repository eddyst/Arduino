#if defined(__AVR_ATtiny13A__) || defined(__AVR_ATtiny13__)
// OW_PORT Pin 6  - PB1
//Analog input PB2

//OW Pin
#define OW_PORT PORTB //1 Wire Port
#define OW_PIN PINB //1 Wire Pin as number
#define OW_PORTN (1<<PINB1)  //Pin as bit in registers
#define OW_PINN (1<<PINB1)
#define OW_DDR DDRB  //pin direction register
#define SET_LOW OW_DDR|=OW_PINN;OW_PORT&=~OW_PORTN;  //set 1-Wire line to low
#define RESET_LOW {OW_DDR&=~OW_PINN;}  //set 1-Wire pin as input
//Pin interrupt	
#define EN_OWINT {GIMSK|=(1<<INT0);GIFR|=(1<<INTF0);}  //enable interrupt 
#define DIS_OWINT  GIMSK&=~(1<<INT0);  //disable interrupt
#define SET_RISING MCUCR=(1<<ISC01)|(1<<ISC00);  //set interrupt at rising edge
#define SET_FALLING MCUCR=(1<<ISC01); //set interrupt at falling edge
#define CHK_INT_EN (GIMSK&(1<<INT0))==(1<<INT0) //test if interrupt enabled
#define PIN_INT ISR(INT0_vect)  // the interrupt service routine
//Timer Interrupt
#define EN_TIMER {TIMSK0 |= (1<<TOIE0); TIFR0|=(1<<TOV0);} //enable timer interrupt
#define DIS_TIMER TIMSK0  &= ~(1<<TOIE0); // disable timer interrupt
#define TCNT_REG TCNT0  //register of timer-counter
#define TIMER_INT ISR(TIM0_OVF_vect) //the timer interrupt service routine

//Times
#define OWT_MIN_RESET 51 //minimum duration of the Reset impulse

#define OWT_RESET_PRESENCE 4 //time between rising edge of reset impulse and presence impulse
#define OWT_PRESENCE 20 //duration of the presence impulse
#define OWT_READLINE 4  //duration from master low to read the state of 1-Wire line
#define OWT_LOWTIME 4 //length of low 

//Initializations of AVR
#define INIT_AVR CLKPR=(1<<CLKPCE);\
				   CLKPR=0;/*9.6Mhz*/\
				   TIMSK0=0;\
				   GIMSK=(1<<INT0);/*set direct GIMSK register*/\
				   TCCR0B=(1<<CS00)|(1<<CS01); /*9.6mhz /64 couse 8 bit Timer interrupt every 6,666us*/

//#ifdef DS18B20
//Setup Temp Measurement DS18B20
#define INIT_TEMP   DDRB&=~(1<<PINB2); \
					ADMUX=(1<<MUX0); \
					ADCSRA= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
#define CONV_TEMP		ADCSRA|=(1<<ADSC); \
						while ((ADCSRA&(1<<ADSC))!=0) {}\
						scratchpad[0]=ADCL;\
						scratchpad[1]=ADCH;\
//#endif
#endif