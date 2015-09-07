#define NotbetriebPin               22

#include <Bounce.h>
#define p Serial.print
void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  pinMode     (NotbetriebPin              , INPUT_PULLUP); //Kein Schalter = Schalter offen = Pin HIGH = Notbetrieb

}

void loop() {
  // put your main code here, to run repeatedly: 
  static Bounce bouncer = Bounce(NotbetriebPin, 30000, true); 
//  static Bounce bouncer = Bounce(NotbetriebPin, 50); 
  p(digitalRead(NotbetriebPin));
  if (bouncer.update()) {
    if (bouncer.fallingEdge()) {
      p(" falling ");
    } 
    else if (bouncer.risingEdge()) {
      p(" rising ");
    } 
  }
    p(bouncer.read());
    p("\n");
        p(bouncer.duration());
    p("\n");
delay(1000);
}
