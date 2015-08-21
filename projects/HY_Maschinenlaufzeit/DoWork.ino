#include <Bounce2.h>
#include <PinChangeInt.h>

Bounce input[pinCount] = Bounce(); 
#define PIN CONTROLLINO_A9  // the pin we are interested in

void DoWorkInit() {
  for (uint8_t pin = 0; pin < pinCount; pin++) {
//    pinMode(A0 + pin ,INPUT_PULLUP);
    pinMode(A0 + pin ,INPUT);
    input[pin].attach(CONTROLLINO_A0 + pin);
    input[pin].interval(100);
  }
//  attachInterrupt(0, zaehler1Interrupt, RISING);
  pinMode(PIN, INPUT);     //set the pin to input
  digitalWrite(PIN, LOW); //do not use the internal pullup resistor
  PCintPort::attachInterrupt(PIN, zaehler1Interrupt, RISING); // attach a PinChange Interrupt to our pin on the rising edge
}

volatile byte zaehler1Lock = 0;
volatile uint32_t zaehler1 = 0;
volatile uint32_t zaehler1zp = 0;
void zaehler1Interrupt(){
  zaehler1Lock ++;
  zaehler1zp = micros();
  zaehler1 ++;
}

#define Intervall 10000 //ms
void DoWork() { 
  static unsigned long LastDoWork;
  if (millis() - LastDoWork > Intervall ) {
    LastDoWork = millis();
    uint32_t z1, z1zp; //Wir holen uns den Wert aus den Interrupts in lokale Variablen
    byte InterruptChecker;
    do {
      InterruptChecker = zaehler1Lock;
      z1   = zaehler1;
      z1zp = zaehler1zp;
    } while (InterruptChecker != zaehler1Lock); //Wenn zaehler1Lock sich geändert hat, ist ein Interrupt dazwischen gekommen 
    #if ethLogLevel > 1
      Debug.print( F("Zaehler1=")); Debug.print( zaehler1); Debug.print( F(", ")); Debug.print( F("Zp=")); Debug.print( zaehler1zp); Debug.println( F(";"));
    #endif
    server.print( F("Zaehler1=")); server.print( zaehler1); server.println( F(";"));
    static uint32_t lastZ1, lastZ1zp;
    uint16_t s;
    if (z1 == lastZ1) {
      s = 0;
      lastZ1zp = LastDoWork * 1000;
    } else {
      #if ethLogLevel > 1
        Debug.print( F("  (z1 - lastZ1)=")); Debug.print( (z1 - lastZ1)); Debug.print( F(", ")); Debug.print( F("(z1zp - lastZ1zp)=")); Debug.print( (z1zp - lastZ1zp)); Debug.println( F(";"));
      #endif
      s = (z1 - lastZ1) * 6000000 / ((z1zp - lastZ1zp) / 10) ;
      lastZ1 = z1;
      lastZ1zp = z1zp;
    }
    #if ethLogLevel > 1
      Debug.print( F("s=")); Debug.print( s); Debug.println( F(";"));
    #endif
    static uint16_t lastS;
    static uint32_t lastSend;
    if (lastS!=s  || (LastDoWork - lastSend) > (60000 - Intervall)){ //Wenn Wert geändert oder 60sek nichts gesendet
      #if ethLogLevel > 1
        Debug.print( F("Speed1=")); Debug.print( s); Debug.println( F(";"));
      #endif
      server.print  ( F("Speed1="));
      server.print  ( s);
      server.println( F(";"));
      lastS = s;
      lastSend = millis();
    }
  }

  for (uint8_t pin = 0; pin < pinCount; pin++) {
    if (input[pin].update()){
      PrintPin(pin);
    }
  }
}

void PrintPin(byte pin) {
      #if ethLogLevel > 1
        Debug.print( F("Pin")); Debug.print( pin); Debug.print( F("=")); Debug.print( input[pin].read()); Debug.println( F(";"));
      #endif
      server.print  ( F("Pin"));
      server.print  ( pin);
      server.print  ( F("="));
      server.print  ( input[pin].read());
      server.println(F(";"));
}

