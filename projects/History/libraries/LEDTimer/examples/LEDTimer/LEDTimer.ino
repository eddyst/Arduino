#include <LEDTimer.h>
//LEDTimer step0(0);
LEDTimer step1(13);

void setup() {
//  pinMode(13 , OUTPUT);
  // put your setup code here, to run once:
//  step0.blink( 100, 200, 10);
  delay(5000);
  Serial.begin( 57600);
  step1.onBlinkEnd( be);
//  step1.blinkBegin( 1000, 1000, 10);
  step1.blinkBegin( 1000);
}

void loop() {
  // put your main code here, to run repeatedly: 
  step1.doEvents();  
}

void be() {
  Serial.println( "Fertsch");
}
