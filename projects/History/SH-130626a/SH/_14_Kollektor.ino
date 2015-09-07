#define PumpeKollektorAnPin         42        
//#define PumpeKollektorServoEnabledPin 
//#define PumpeKollektorServoPwmPin     

//#define PumpeKollektorServoPwmMin 30
//#define PumpeKollektorServoPwmMax 175
#define KollektorTryIntervall 120000

//#include <Servo.h> //
//Servo servoKollektor;  // create servo object to control a servo a maximum of eight servo objects can be created 

uint32_t Stagnation = 0;

void   KollektorInit() {
  pinMode     (PumpeKollektorAnPin, OUTPUT);   
  digitalWrite(PumpeKollektorAnPin, LOW   );

  //  servoKollektor.attach(PumpeKollektorServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  pinMode     (PumpeKollektorServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeKollektorServoEnabledPin, LOW   );

  for (uint8_t i = 0; i < 4; i++) {
    Stagnation = (Stagnation << 8) | EEPROM.read(EEPROM_Offset_Stagnation + i);
  }
}

#define KollektorStatus_UNKNOWN_Uhrzeit              -40
#define KollektorStatus_UNKNOWN_KollektorWTRuecklauf -30
#define KollektorStatus_UNKNOWN_SpeicherA5           -20
#define KollektorStatus_Stagnation                   -10
#define KollektorStatus_AUS                            0
#define KollektorStatus_TRY                           10
#define KollektorStatus_AN                            20
#define KollektorStatus_AUSschalten                   30

void KollektorDoEvents (){
  uint32_t zp = g_Clock.GetTimestamp();
  static uint32_t millis1 = millis();
  uint8_t Status = KollektorStatus_UNKNOWN_Uhrzeit;
  if (Values[_KollektorStatus].ValueX10 != ValueUnknown) 
    Status = Values[_KollektorStatus].ValueX10;
  if (Status > KollektorStatus_Stagnation 
    && Values[_KollektorWTRuecklauf].ValueX10 >  800         ) { //Es wird zu warm 
    Stagnation = zp / 86400 * 86400 + 111600;                   // (Timestamp / 86400(Sekunden pro Tag)) gerundeter  * 86400(Sekunden pro Tag) = Tagesanfang + 24h = morgen + 7h = morgen um 7 ist Kollektor wieder freigegeben
    digitalWrite(PumpeKollektorAnPin, LOW);
    for (uint8_t i = 0; i < 4; i++) {
      EEPROM.write(EEPROM_Offset_Stagnation + 3 - i, (Stagnation >> (8 * i)) & 255);
    }
    Status = KollektorStatus_Stagnation;
  }
  switch (Status){
  case KollektorStatus_UNKNOWN_Uhrzeit:
    if (zp > 10000000) 
      Status = KollektorStatus_UNKNOWN_KollektorWTRuecklauf;
    break;
  case KollektorStatus_UNKNOWN_KollektorWTRuecklauf:
    if (Values[_KollektorWTRuecklauf].ValueX10 != ValueUnknown) 
      Status = KollektorStatus_UNKNOWN_SpeicherA5;
    break;
  case KollektorStatus_UNKNOWN_SpeicherA5:
    if (Values[_SpeicherA5].ValueX10 != ValueUnknown) 
      Status = KollektorStatus_Stagnation;
    break;
  case KollektorStatus_Stagnation:
    if (zp > Stagnation)
      millis1 = millis() - KollektorTryIntervall;  // Wir brauchen jetzt keine zusätzliche Wartezeit
      Status = KollektorStatus_AUS;
    break;
  case KollektorStatus_AUS: //Anschalten?
    if (millis() - millis1 > KollektorTryIntervall) {
      digitalWrite(PumpeKollektorAnPin, HIGH);
      millis1 = millis();
      Status = KollektorStatus_TRY;
    }
    break;
  case KollektorStatus_TRY: //Anschalten?
    if ( Values[_KollektorWTRuecklauf].ValueX10 > Values[_SpeicherA5].ValueX10 + 5)
      Status = KollektorStatus_AN;
    else
      if (millis() - millis1 > 60000){
        Status = KollektorStatus_AUSschalten;
      }
    break;
  case KollektorStatus_AN:
    if (Values[_SolarVorlauf].ValueX10 < Values[_SpeicherA5].ValueX10){// Uppps Wärme ist weg
      Status = KollektorStatus_AUSschalten;
    }  
    break;
  }
  if (Status == KollektorStatus_AUSschalten) {
    digitalWrite(PumpeKollektorAnPin, LOW);
    millis1 = millis();
    Status = KollektorStatus_AUS;
  }
  if (Values[_KollektorStatus].ValueX10 != Status) {
    Values[_KollektorStatus].ValueX10 = Status;
    Values[_KollektorStatus].Changed  = 1;
    if (kollLogLevel > 0) {
                Debug.print(F("Koll: Status Zugewiesen: "));           
                Debug.println(Status); 
              }

  }
}









