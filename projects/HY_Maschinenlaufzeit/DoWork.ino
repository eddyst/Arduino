#include <Bounce2.h>

const uint8_t pinCount = 1;
Bounce input[pinCount] = Bounce(); 

void DoWorkInit() {
  attachInterrupt(0, zaehler1Interrupt, RISING);
  
  for (uint8_t pin = 0; pin < pinCount; pin++) {
    pinMode(A0 + pin ,INPUT_PULLUP);
    input[pin].attach(A0 + pin);
    input[pin].interval(500);
  }
}

volatile uint32_t zaehler1 = 0;
void zaehler1Interrupt()
{
  zaehler1 ++;
}

#define Intervall 10000000 //ns
void DoWork() { 
  static unsigned long LastDoWork;
  if (micros() - LastDoWork > Intervall ) {
/*    #if aDCLogLevel > 1
      Debug.print(i); 
      Debug.print(F(": ")); 
      server.println(i);
    #endif */
  }

  for (uint8_t pin = 0; pin < pinCount; pin++) {
    if (input[pin].update()){
      server.print  ( F("Pin"));
      server.print  ( pin);
      server.print  ( F("="));
      server.println( input[pin].read());
    }
  }
}

