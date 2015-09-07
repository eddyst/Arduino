#define NotbetriebPin               22

#include <Bounce.h>

void AllgInit() {
  pinMode     (NotbetriebPin              , INPUT_PULLUP); //Kein Schalter = Schalter offen = Pin HIGH = Notbetrieb
}

void   AllgDoEvents() {
  pruefeNotbetriebPin();
}

void   pruefeNotbetriebPin() {
  static Bounce bouncer = Bounce(NotbetriebPin, 500); 
  if (bouncer.update()) {
    if (bouncer.fallingEdge()) {
      Solarbetrieb = true;
      ThermeSolarbetriebBeginnt();
    } 
    else if (bouncer.risingEdge()) {
      Solarbetrieb = false;
      ThermeSolarbetriebEndet();
    } 
  }
}








