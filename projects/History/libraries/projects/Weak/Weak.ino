//#include <arduino.h>
#include <Weak.h>

Weak w;
void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  Serial.begin( 57600);
}

void loop() {
  // put your main code here, to run repeatedly: 
  w.ping(millis());
}

void pongEvent(uint8_t) {
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(1000);
}
void debugEvent(char Msg[]) {
  Serial.println(Msg);
}
