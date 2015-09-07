#include "OneWireSlave.h"
//                     {Fami, ----, ----, ----, -ID-, ----, ----,  CRC} 
unsigned char rom[8] = {0x28, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
//                            {TLSB, TMSB, THRE, TLRE, Conf, 0xFF, Rese, 0x10,  CRC}
unsigned char scratchpad[9] = {0xB4, 0x09, 0x4B, 0x46, 0x1F, 0xFF, 0x00, 0x10, 0x00};

OneWireSlave ds(2);

const int ledPin =  13;         // the number of the LED pin
// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 250;           // interval at which to blink (milliseconds)

void setup() {
  pinMode(ledPin, OUTPUT);
  ds.init(rom);
  ds.setScratchpad(scratchpad);
}

void loop() {
  blinking();
  ds.waitForRequest(false);
}

void blinking() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis; 
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}
