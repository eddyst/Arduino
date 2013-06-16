#define PumpeSolarAnPin             38              
//#define PumpeSolarServoEnabledPin 46
//#define PumpeSolarServoPwmPin 48
//#define PumpeSolarServoPwmMin 30
//#define PumpeSolarServoPwmMax 175
#define PumpeKollektorAnPin         42        

//#include <Servo.h> //
//Servo servoSolar;  // create servo object to control a servo a maximum of eight servo objects can be created 

uint32_t Stagnation = 0;

void   SolarInit() {
  pinMode     (PumpeSolarAnPin    , OUTPUT);   
  digitalWrite(PumpeSolarAnPin    , LOW   );

  pinMode     (PumpeKollektorAnPin, OUTPUT);   
  digitalWrite(PumpeKollektorAnPin, LOW   );

  //  servoSolar.attach(PumpeSolarServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  pinMode     (PumpeSolarServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeSolarServoEnabledPin, LOW   );

  for (uint8_t i = 0; i < 4; i++) {
    Stagnation = (Stagnation << 8) | EEPROM.read(EEPROM_Offset_Stagnation + i);
  }
}
void schaltePumpeKollektor(boolean AnAus) {
  digitalWrite(PumpeKollektorAnPin, LOW);
}
void SolarDoEvents (){
  uint32_t zp = g_Clock.GetTimestamp();
    if (zp > Stagnation
      && Values[_KollektorWTRuecklauf].ValueX10 != ValueUnknown){
      if (Values[_KollektorWTRuecklauf].ValueX10 > 800) { //Es wird zu warm 
        Stagnation = zp / 86400 * 86400 + 111600; // (Timestamp / 86400(Sekunden pro Tag)) gerundeter  * 86400(Sekunden pro Tag) = Tagesanfang + 24h = morgen + 7h = morgen um 7 ist Solar wieder freigegeben
        schaltePumpeKollektor(LOW);
        for (uint8_t i = 0; i < 4; i++) {
          EEPROM.write(EEPROM_Offset_Stagnation + 4 - i, (Stagnation >> (8 * i)) & 255);
        }
      } 
      else {
        DateTime dt = g_Clock.GetDateTime(zp);
        if (dt.Hour > 7) {
          if (Values[_KollektorWTRuecklauf].ValueX10 > Values[_SpeicherA5].ValueX10 + 5){}
          }
        }
      }
    /*
  static uint8_t pos = 0;    // variable to store the servo position 
     static uint32_t lastMove = 0;
     if (millis() - lastMove > 1000) {
     if (pos++ > PumpeSolarServoPwmMax) 
     pos = 0; 
     else
     pos++;
     if (solarLogLevel > 0) {
     Debug.print(F("pump: Pos = "));
     Debug.println(pos);
     }
     //    servoSolar.write(pos);
     lastMove = millis();
     }
     */
  }





