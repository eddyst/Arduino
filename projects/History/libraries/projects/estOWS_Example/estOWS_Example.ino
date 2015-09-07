#include "estOWS.h"


estOWS ow(0, 4); // (interrupt, idCount)

void setup() {
  Serial.begin(57600);
  ow.setIdList(1, (uint8_t []){ 0xE1, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x02});
  ow.setIdList(2, (uint8_t []){ 0x28, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x03});
  ow.setIdList(2, (uint8_t []){ 0x28, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x04});
  ow.setIdList(2, (uint8_t []){ 0x28, 0xA2, 0xD9, 0x84, 0x00, 0x00, 0x05});
  ow.attachOnCmd(onCmd);
}

void loop() {
  // put your main code here, to run repeatedly: 
  pinMode(13,OUTPUT);
}

void onCmd (uint8_t cmd) {
  digitalWrite(13, !digitalRead(13));
}
