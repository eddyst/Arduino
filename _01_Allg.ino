#define NotbetriebPin               22

#include <Bounce.h>
#include <Clock.h>
Clock g_Clock;

void AllgInit() {
  pinMode     (NotbetriebPin              , INPUT_PULLUP); //Kein Schalter = Schalter offen = Pin HIGH = Notbetrieb
  g_Clock.SetTimezoneOffset(2,0); // Setup the clock. In Germany we are 1 hours ahead of GMT  (2 zur Sommerzeit)
}

void   AllgDoEvents() {
  g_Clock.Maintain();
  pruefeNotbetriebPin();
}

void   pruefeNotbetriebPin() {
  static Bounce bouncer = Bounce(NotbetriebPin, 500, true); 
  if (bouncer.update()) {
    if (bouncer.fallingEdge()) {
      ThermeSolarbetriebBeginnt();
      HKSolarbetriebBeginnt();    
    } 
    else if (bouncer.risingEdge()) {
      ThermeSolarbetriebEndet();
      HKSolarbetriebEndet();
    } 
  }
}


boolean setValue( uint8_t valueID, int16_t value) {
  if (Values[valueID].ValueX10 != value) {
    Values[valueID].ValueX10 = value;
    Values[valueID].Changed  = 1;
    return true;
  }  
  else {
    return false;
  }
}

// Converts an ASCII character to a numeric value.
// Returns true if the character is numeric, returns
// false for all other characters.
// Utilises the fact that char type is just a byte.
// Thus '7' (ASCII 55) minus '0' (ASCII 48) equals 7.
boolean parseHexChar(const char& input, uint8_t& output) {
  if(input >= '0' && input <= '9') {
    output = input - '0';
    return true; 
  } 
  else if(input >= 'A' && input <= 'F'){
    output = input - 'A' + 10;
    return true; 
  } 
  else if(input >= 'a' && input <= 'f'){
    output = input - 'a' + 10;
    return true; 
  }
  output = 0;
  return false;
}










