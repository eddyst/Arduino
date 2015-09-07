/*
 *  Interrupt and PWM utilities for 16 bit Timer2 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified Oct 2009 by Dan Clemens to work with timer2 of the ATMega1280 or Arduino Mega
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 

#include "estTimer2.h"

//estTimer2 Timer2;              // preinstatiate
estTimer2 * estTimer2::me;

ISR(TIMER2_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  estTimer2::isr();
}
void estTimer2::isr(){
        me->restart();
	me->isrCallback();
}
estTimer2::estTimer2() {
	me = this;
}
void estTimer2::initialize(void (*isr)(), long microseconds)
{
  TCCR2A = 0;                 // clear control register A 
  TCCR2B = _BV(WGM13);        // set mode as phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  TIMSK2 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
}

void estTimer2::setPeriod(long microseconds)
{
  long cycles = (F_CPU * microseconds) / 2000000;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
//  ICR2 = cycles;                                                                 // ICR1 is TOP in p & f correct pwm mode
  TCCR2B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR2B |= clockSelectBits;                                                     // reset clock select register
}

void estTimer2::start()
{
  TCCR2B |= clockSelectBits;
}

void estTimer2::stop()
{
  TCCR2B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
}

void estTimer2::restart()
{
  TCNT2 = 0;
}
*/
