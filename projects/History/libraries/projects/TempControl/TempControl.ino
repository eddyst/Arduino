#define owLogLevel 9
#define Debug Serial

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX

#define EEPROM_Offset_owArray 0
//#define PinOneWireSlave 2
#define PinKaliKalt   2
#define PinKaliWarm   3

volatile uint32_t posSoll = 0;

volatile uint32_t pos     = 1;
uint32_t Schritte         = 638559;
int32_t SFehlerMin       = 0;
int32_t SFehlerMax       = 0;
uint8_t  Status           = 0;
#define Kalibrieren 0
#define Kalibriere1 1
#define Kalibriere2 2
#define Kalibriere3 3
#define OK          4

volatile uint8_t kali = 0;
#define KaliKalt 1
#define KaliWarm 2

#define ROT  7
#define BLAU 8

uint16_t V1 = 0;
uint16_t V2 = 0;
uint16_t R  = 0;
uint16_t ZielTemp = 30 * 16;

uint8_t Addr[] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    //V1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    //V2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }; //R
char sBuffer[10];

void setup() {
  Debug.begin(57600);
  Debug.println( F( "Start"));
  pinMode(PinKaliKalt       , INPUT);
  pinMode(PinKaliWarm       , INPUT);
  pinMode(ROT  , OUTPUT);
  pinMode(BLAU , OUTPUT);
  mySerial.begin(9600);
  initStepper();
}

void loop() {
  while (mySerial.available()) {
    Debug.print("mySerial-");
    doChar (mySerial.read()); 
  }
  bitWrite( kali, KaliKalt, digitalRead( PinKaliKalt));
  bitWrite( kali, KaliWarm, digitalRead( PinKaliWarm));
  digitalWrite ( BLAU, bitRead( kali, KaliKalt));
  digitalWrite ( ROT , bitRead( kali, KaliWarm));
#define posIntervall 10000  // 10Sek
  static uint32_t posLastMillis ;
  switch (Status) {
  case Kalibrieren:
    setPos(0);
    Status=Kalibriere1;
    Debug.println( F( "Status K1"));
    break;
  case Kalibriere1:
    if (TIMSK2 == 0) {
      Status=Kalibriere2;
      Debug.println( F( "Status K2"));
    }
    break;
  case Kalibriere2:
    setPos(Schritte);
    Status=Kalibriere3;
    Debug.println( F( "Status K3"));
    break;
  case Kalibriere3:
    if (TIMSK2 == 0) {
      posLastMillis = millis() - posIntervall;
      Status=OK;
      Debug.println( F( "Status OK"));
    }
    break;
  case OK:
    /*  
     if (millis() - posLastMillis >= posIntervall ) {
     posLastMillis += posIntervall;
     if (V1<ZielTemp && R < ZielTemp) {
     setPos (Schritte);
     }
     else if (V1 > ZielTemp && R > ZielTemp) {
     setPos (0);
     } 
     else {
     #define Fak 1000 //Vermeidet Rundungsdifferenzen beim dividieren
     uint32_t bPos, iPos, sPos;
     if (ZielTemp == R) { //Division durch null abfangen
     bPos = 0;
     } 
     else {
     bPos = Schritte * Fak / ( Fak + Fak * ( V1 - ZielTemp) / ( ZielTemp - R));
     }
     if (V2 == R) { //Division durch null abfangen
     iPos = 0;
     } 
     else {
     iPos = Schritte * Fak / ( Fak + Fak * ( V1 - V2      ) / ( V2       - R));
     }
     sPos = sqrt(pos * abs(pos + 0.001 * (bPos - iPos)));
     setPos ( sPos);
     Debug.print  ( F ("bPos "));
     Debug.print  (     bPos   );
     Debug.print  ( F (" iPos "));
     Debug.print  (      iPos   );
     Debug.print  ( F (" pos "));
     Debug.print  (      pos   );
     Debug.print  ( F (" -> "));
     Debug.println( sPos);
     }
     }
     */
    break;
  }
  static uint32_t t = millis()-2000;
  if (millis()-t > 2000) {
    t = millis();
    Debug.print  ( F ("pos "));
    Debug.print  (     pos   );
    Debug.print  ( F ("; posSoll "));
    Debug.print  (     posSoll   );
    Debug.print  ( F ("; Schritte "));
    Debug.print  (     Schritte   );
    Debug.print  ( F ("; SFehlerMin "));
    Debug.print  (     SFehlerMin   );
    Debug.print  ( F ("; SFehlerMax "));
    Debug.print  (     SFehlerMax   );
    Debug.print  ( F ("; Status "));
    Debug.print  (     Status   );
    Debug.print  ( F ("; kali "));
    Debug.print  (     kali   );
    if (!bitRead( kali, KaliKalt)) 
      Debug.print  ( F (" notKalt "));
    else
      Debug.print  ( F ("    Kalt "));
    Debug.println();
  }

}


void serialEvent() {
  while (Serial.available()) {
    Debug.print("serialEvent-");
    doChar ( Serial.read()); 
  }
}

void doChar( char inChar) {
  static uint16_t x = 0;                                                                                                                                                                       
  Debug.print(F("IN:")); Debug.println(inChar);
  if ( Status == OK) {
    switch (inChar) {
    case '0':
      x = x * 10 + 0;
      break;
    case '1':
      x = x * 10 + 1;
      break;
    case '2':
      x = x * 10 + 2;
      break;
    case '3':
      x = x * 10 + 3;
      break;
    case '4':
      x = x * 10 + 4;
      break;
    case '5':
      x = x * 10 + 5;
      break;
    case '6':
      x = x * 10 + 6;
      break;
    case '7':
      x = x * 10 + 7;
      break;
    case '8':
      x = x * 10 + 8;
      break;
    case '9':
      x = x * 10 + 9;
      break;
    case '#':
      setPos(Schritte / 1000 * x);
      x=0;
      break;
    } 
  }
}





