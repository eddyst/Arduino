#define NotbetriebPin               22

#include <Bounce.h>
#include <Clock.h>
Clock g_Clock;

void AllgInit() {
  pinMode     (NotbetriebPin              , INPUT_PULLUP); //Kein Schalter = Schalter offen = Pin HIGH = Notbetrieb
  g_Clock.SetTimezoneOffset(1,0); // Setup the clock. In Germany we are 1 hours ahead of GMT 
}

void   AllgDoEvents() {
  g_Clock.Maintain();
  pruefeNotbetriebPin();
}

void   pruefeNotbetriebPin() {
  static Bounce bouncer = Bounce(NotbetriebPin, 500, true); 
  if (bouncer.update()) {
    if (bouncer.fallingEdge()) {
      Solarbetrieb = true;
      ThermeSolarbetriebBeginnt();
      HKSolarbetriebBeginnt();    
    } else if (bouncer.risingEdge()) {
      Solarbetrieb = false;
      ThermeSolarbetriebEndet();
      HKSolarbetriebEndet();
    } 
  }
}









