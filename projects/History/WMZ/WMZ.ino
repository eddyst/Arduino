#define Debug Serial
#define owLogLevel 1
#include <EEPROM.h>

typedef struct {
  //  uint8_t Changed  ;  int16_t ValueX10 ;
uint8_t Read     : 
  1;
uint8_t Changed  :   
  1;
int16_t ValueX10 :  
  15;
} 
data;

#define _Vorlauf          0
#define _Ruecklauf        1
#define _WattCounterMinus 2
#define _WattCounterPlus  3

#define ValueUnknown -10000
data Values[] = {
  { 
    0, ValueUnknown    }
  , 
  { 
    0, ValueUnknown    }
  ,  
  { 
    0, 0               }
  ,  
  { 
    0, 0               }
}; 

#define EEPROM_Offset_owArray    100
uint8_t  owArray[] = { 
  _Vorlauf, _Ruecklauf};

#define z1Pin 0
volatile int32_t impulseMalDeltaTMinus = 0;
volatile int32_t impulseMalDeltaTPlus = 0;
volatile boolean impulseMalDeltaTChanged = 0;
volatile int16_t  deltaT[2]           = {0,0};
volatile boolean  deltaTSelector      = 0;

void setup() {
  Serial.begin(57600);
  pinMode(9     , OUTPUT);
  pinMode(13    , OUTPUT);
  attachInterrupt(z1Pin, z1, FALLING);
  E(0,0, 0x28); 
  E(0,1, 0x23); 
  E(0,2, 0x6A); 
  E(0,3, 0xAA); 
  E(0,4, 0x03); 
  E(0,5, 0x00); 
  E(0,6, 0x00); 
  E(0,7, 0x0B);

  E(1,0, 0x28); 
  E(1,1, 0x29); 
  E(1,2, 0xA8); 
  E(1,3, 0xAA); 
  E(1,4, 0x03); 
  E(1,5, 0x00); 
  E(1,6, 0x00); 
  E(1,7, 0x65); 
}

void E(uint8_t a, uint8_t b, uint8_t data) {
  if ( data != EEPROM.read( EEPROM_Offset_owArray + a * 8 + b)) {
    Debug.print ( F("Set EEPROM "));
    EEPROM.write( EEPROM_Offset_owArray + a * 8 + b, data);
  }
}
uint32_t tmpMinus = 0, tmpPlus = 0;
void loop() {
  doEventsOWMaster();
  // liter * DeltaT * 4,19 / 3600 = kWh
  // 237.4 * (36.7 - 32.4) *4,19 / 3600 = 1188 W
  do { 
  impulseMalDeltaTChanged = 0;
  tmpMinus = impulseMalDeltaTMinus;
  tmpPlus = impulseMalDeltaTPlus;
  } while ( impulseMalDeltaTChanged == 1);
 
  if (setValue( _WattCounterMinus, tmpMinus / 224.954327092207)) {
    Debug.print ( F("Set Watt "));
    Debug.println ( Values[_WattCounterPlus].ValueX10);
  }
  if (setValue( _WattCounterPlus, tmpPlus / 224.954327092207)) {
    Debug.print ( F("Set Watt "));
    Debug.println ( Values[_WattCounterPlus].ValueX10);
  }
  static uint32_t t2 = 0;
  if ( millis() - t2 > 10000) {
    t2=millis();
    digitalWrite (9, HIGH);
    digitalWrite (9, LOW);
  }
  Loging();
}

void z1() {
  impulseMalDeltaTChanged = 1;
  if (deltaT > 0)
    impulseMalDeltaTPlus += deltaT[deltaTSelector];
  else
    impulseMalDeltaTMinus -= deltaT[deltaTSelector];
  digitalWrite (13, !digitalRead(13));
}

void setDeltaT (uint16_t T) {
  deltaT[!deltaTSelector] = T;
  deltaTSelector = !deltaTSelector;
}

boolean setValue( uint8_t valueID, int16_t value) {
  Values[valueID].Read = 1;
  if (Values[valueID].ValueX10 != value) {
    Values[valueID].ValueX10 = value;
    Values[valueID].Changed  = 1;
    return true;
  }  
  else {
    return false;
  }
}

void Loging() {
  static uint32_t t = 0;
  if ( millis() - t > 2000) {
    t=millis();
    Debug.print ( F(" Vorlauf = "));
    Debug.print ( Values[_Vorlauf].ValueX10);
    Debug.print ( F(", Ruecklauf = "));
    Debug.print ( Values[_Ruecklauf].ValueX10);
    Debug.print ( F(", deltaT = "));
    Debug.print ( deltaT[deltaTSelector] );
    Debug.print ( F(", impulseMalDeltaT = "));
    Debug.print ( impulseMalDeltaTPlus );
    Debug.print ( F(", Watt = "));
    Debug.println ( Values[_WattCounterPlus].ValueX10);
  }
}
