#include <arduino.h>
#include <estTimer2.h>

class estOWS {
public:
  estOWS(int interrupt, int IDCount);
  ~estOWS();
  void setIdList(uint8_t Index, uint8_t newIdList[8]);
  void attachOnCmd(void (*)(uint8_t));
private:
  uint8_t  idCount;  uint8_t* idList;
  uint8_t interrupt;
  void (* onCmd)(uint8_t);
  static uint8_t crc8(uint8_t data[], uint8_t len);

  static estOWS *me;
  void PIN_INT();
  static void PIN_INT_Static();
  void set_PIN_INT(uint8_t mode);
};

estOWS::estOWS(int interrupt, int newIDCount) {
  me=this;
  this->interrupt = interrupt;
  idCount = newIDCount;
  idList = new uint8_t[newIDCount * 8];
  set_PIN_INT( FALLING);
  Serial.println("attached");
}
estOWS::~estOWS() {
  delete[] idList;
}

void estOWS::setIdList(uint8_t Index, uint8_t IdList[7]) {
  if (Index < idCount) {
    for (int i =0; i<7; i++) {
      this->idList[ Index * 8 + i] = IdList[i];
    }
    this->idList[ Index * 8 + 7] = crc8(idList, 7);
  }
}

#define DISABLE 0 // CHANGE 1, FALLING 2, RISING 3
void estOWS::set_PIN_INT(uint8_t mode) {
switch (mode) {
case FALLING: attachInterrupt( interrupt, PIN_INT_Static, FALLING);
case RISING: attachInterrupt( interrupt, PIN_INT_Static, RISING);
case DISABLE: detachInterrupt( interrupt);
}}

void estOWS::attachOnCmd(void (* func)(uint8_t)){
  onCmd = func;
}
  
uint8_t estOWS::crc8(uint8_t data[], uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t inbyte = *data++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
        inbyte >>= 1;
    }
  }
  return crc;
}

estOWS * estOWS::me;
void estOWS::PIN_INT() {
  onCmd(0x44);
}

void estOWS::PIN_INT_Static() {
  me->PIN_INT();
}

