#include <OneWireSlave.h>

//                     {Fami, <---, ----, ----, ID--, ----, --->,  CRC} 
unsigned char rom[8] = {0x28, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
//                            {TLSB, TMSB, THRH, TLRL, Conf, 0xFF, Rese, 0x10,  CRC}
unsigned char scratchpad[9] = {0x7F, 0x09, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};
//                                       {TLSB, TMSB}
unsigned char scratchpadtemperature[2] = {0x7F, 0x09};

OneWireSlave ds(2);

//Interrupt
volatile long previousmicros = 0;
volatile long old_previousmicros = 0;
volatile long difference = 0;

//Blinking
const int ledPin =  13;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 750;            // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  ds.init(rom);
  ds.setScratchpad(scratchpad);
  ds.setPower(PARASITE);
  ds.setResolution(9);
  attachInterrupt(1, DS18B20, CHANGE);
  ds.attach44h(temper);
}

void loop() {
  //blinking();
  //ds.waitForRequest(false);
}

void temper() {
  scratchpadtemperature[0] = scratchpadtemperature[0] + 1;
  ds.setTemperature(scratchpadtemperature);
}

void DS18B20() {
  old_previousmicros = previousmicros;
  previousmicros = micros();
  difference = previousmicros - old_previousmicros;
  if (difference >= 380 && difference <= 800) {
    ds.waitForRequestInterrupt(false);
  }
}

void blinking() {
  unsigned long currentMillis = millis(); 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}
