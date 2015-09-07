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

