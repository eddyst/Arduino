#ifndef estTimer2_h
#define estTimer2_h

#ifdef __AVR__
#include <avr/interrupt.h>
#else
#error estTimer2 library only works on AVR architecture
#endif


namespace estTimer2 {
	extern unsigned long time_units;
	extern void (*func)();
	extern volatile unsigned long count;
	extern volatile char overflowing;
	extern volatile unsigned int tcnt2;
	
	void set(unsigned long ms, void (*f)());
	void set(unsigned long units, double resolution, void (*f)());
	void start();
	void stop();
        void setPeriod(long microseconds);
        void _overflow();
}

#endif


