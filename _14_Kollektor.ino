#define KollektorPumpeAnPin         34        

#define KollektorTryIntervall           360000
#define KollektorTryDauer                60000
#define KollektorWartezeitNachAnBevorAus 60000

//#include <Servo.h> //
//Servo servoKollektor;  // create servo object to control a servo a maximum of eight servo objects can be created 

uint32_t Stagnation = 0;

void   KollektorInit() {
  pinMode     (KollektorPumpeAnPin, OUTPUT);   
  digitalWrite(KollektorPumpeAnPin, LOW   );

  //  servoKollektor.attach(PumpeKollektorServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  pinMode     (PumpeKollektorServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeKollektorServoEnabledPin, LOW   );

  for (uint8_t i = 0; i < 4; i++) {
    Stagnation = (Stagnation << 8) | EEPROM.read(EEPROM_Offset_Stagnation + i);
  }
}

#define KollektorStatus_UNKNOWN_Uhrzeit              -40
#define KollektorStatus_UNKNOWN_KollektorWTVorlauf   -30
#define KollektorStatus_UNKNOWN_SpeicherA5           -20
#define KollektorStatus_Stagnation                   -10
#define KollektorStatus_AUS                            0
#define KollektorStatus_TRY                           10
#define KollektorStatus_AN                            20
#define KollektorStatus_AUSschalten                   30

void KollektorDoEvents (){
  uint32_t zp = g_Clock.GetTimestamp();
  static uint32_t millis1 = millis();
  static int8_t Status = KollektorStatus_UNKNOWN_Uhrzeit;
  if (Status > KollektorStatus_Stagnation 
    && Values[_KollektorWTVorlauf].ValueX10 >  900         ) { //Es wird zu warm 
    Stagnation = zp / 86400 * 86400 + 111600;                  // (Timestamp / 86400(Sekunden pro Tag)) gerundeter  * 86400(Sekunden pro Tag) = Tagesanfang + 24h = morgen + 7h = morgen um 7 ist Kollektor wieder freigegeben
    digitalWrite(KollektorPumpeAnPin, LOW);
    for (uint8_t i = 0; i < 4; i++) {
      EEPROM.write(EEPROM_Offset_Stagnation + 3 - i, (Stagnation >> (8 * i)) & 255);
    }
    Status = KollektorStatus_Stagnation;
    if( kollLogLevel > 0) {
      Debug.print( F("Koll: Stagnationsroutine hat Pumpe deaktiviert - Neuer Status: "));           
      Debug.println(Status); 
    }
  }
 
  switch (Status){
  case KollektorStatus_UNKNOWN_Uhrzeit:
    if (zp > 10000000) 
      Status = KollektorStatus_UNKNOWN_KollektorWTVorlauf;
    break;
  case KollektorStatus_UNKNOWN_KollektorWTVorlauf:
    if (Values[_KollektorWTVorlauf].ValueX10 != ValueUnknown) 
      Status = KollektorStatus_UNKNOWN_SpeicherA5;
    break;
  case KollektorStatus_UNKNOWN_SpeicherA5:
    if (Values[_SpeicherA5].ValueX10 != ValueUnknown) 
      Status = KollektorStatus_Stagnation;
    break;
  case KollektorStatus_Stagnation:
    if (zp > Stagnation) {
      millis1 = millis() - KollektorTryIntervall;  // Wir brauchen jetzt keine zusätzliche Wartezeit
      Status = KollektorStatus_AUS;
    }
    break;
  case KollektorStatus_AUS: //Anschalten?
    if (millis() - millis1 > KollektorTryIntervall) {
      digitalWrite(KollektorPumpeAnPin, HIGH);
      millis1 = millis();
      Status = KollektorStatus_TRY;
    }
    break;
  case KollektorStatus_TRY: //Anschalten?
    if ( Values[_KollektorWTVorlauf].ValueX10 > Values[_SpeicherA5].ValueX10 + 50) {
      millis1 = millis();
      Status = KollektorStatus_AN;
    } 
    else if (millis() - millis1 > KollektorTryDauer){
      Status = KollektorStatus_AUSschalten;
    }
    break;
  case KollektorStatus_AN:
    if (millis() - millis1 > KollektorWartezeitNachAnBevorAus
      && Values[_KollektorWTVorlauf].ValueX10 < Values[_SpeicherA5].ValueX10 + 5){// Uppps Wärme ist weg
      Status = KollektorStatus_AUSschalten;
    }
    break;
  }
  if (Status == KollektorStatus_AUSschalten) {
    digitalWrite(KollektorPumpeAnPin, LOW);
    millis1 = millis();
    Status = KollektorStatus_AUS;
  }
  if ( setValue( _KollektorStatus, Status) && kollLogLevel > 0) {
    Debug.print( F("Koll: Status Zugewiesen: "));           
    Debug.println(Status); 
  }
}











