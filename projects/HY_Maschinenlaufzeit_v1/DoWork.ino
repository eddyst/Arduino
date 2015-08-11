#include <Bounce2.h>

const uint8_t pinCount = 1;
Bounce input[pinCount] = Bounce(); 

void DoWorkInit() {
  attachInterrupt(0, blink, RISING);
  for (uint8_t pin = 0; pin < pinCount; pin++) {
    pinMode(A0 + pin ,INPUT_PULLUP);
    debouncer.attach(A0 + pin);
    debouncer.interval(500);
  }
}

void blink()
{
  zaehler1 ++;
}

void DoWork() { 
  static unsigned long LastDoWork;
  if (micros() - LastDoWork > (Intervall * 1000 / 60)) {
    #if aDCLogLevel > 1
      Debug.print(i); 
      Debug.print(F(": ")); 
    #endif 
  }
}

void DoWorkPinChange() {
  for (uint8_t pin = 0; pin < pinCount; pin++) {
    if (input[pin].update()){
      
      SendPinChangeEvent (pin, input[pin].read());
    }
  }
}

