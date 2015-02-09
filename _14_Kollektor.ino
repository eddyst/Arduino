#define KollektorPumpeAnPin         34        

#define KollektorTryIntervall            330000 //9,5 min
#define KollektorTryDauer                 30000 //30 sec
#define KollektorWartezeitNachAnBevorAus 120000

uint32_t KollektorGesperrtBisZp = 0;

void   KollektorInit() {
  pinMode     (KollektorPumpeAnPin, OUTPUT);   
  digitalWrite(KollektorPumpeAnPin, LOW   );

  for (tmpUint8_1 = 0; tmpUint8_1 < 4; tmpUint8_1++) {
    KollektorGesperrtBisZp = (KollektorGesperrtBisZp << 8) | EEPROM.read(EEPROM_Offset_Stagnation + tmpUint8_1);
  }
}

void KollektorDoEvents (){
  UhrZp = Uhr.GetTimestamp();
  static uint32_t millis1 = millis();
  static int8_t Status = KollektorStatus_UNKNOWN_Uhrzeit;
  if (Status > KollektorStatus_Stagnation 
    && Values[_KollektorWTVorlauf].ValueX10 > 900         ) {             //Es wird zu warm 
    KollektorGesperrtBisZp = UhrZp / 86400 * 86400 + 111600;                  // (Timestamp / 86400(Sekunden pro Tag)) gerundeter  * 86400(Sekunden pro Tag) = Tagesanfang + 24h = morgen + 7h = morgen um 7 ist Kollektor wieder freigegeben
    digitalWrite(KollektorPumpeAnPin, LOW);
    for (tmpUint8_1 = 0; tmpUint8_1 < 4; tmpUint8_1++) {
      EEPROM.write(EEPROM_Offset_Stagnation + 3 - tmpUint8_1, (KollektorGesperrtBisZp >> (8 * tmpUint8_1)) & 255);
    }
    Status = KollektorStatus_Stagnation;
    if( kollLogLevel > 0) {
      Debug.print( F("Koll: Stagnationsroutine hat Pumpe deaktiviert - Neuer Status: "));           
      Debug.println(Status); 
    }
  }
 
  switch (Status){
  case KollektorStatus_UNKNOWN_Uhrzeit:
    if (UhrZp > 10000000) 
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
  case KollektorStatus_Sperrzeit:
  case KollektorStatus_Stagnation:
    if (UhrZp > KollektorGesperrtBisZp) {
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
    if (Uhr.GetDateTime(UhrZp).Hour > 19) { // Wenn es schon nach 20Uhr ist, wird keine Wärme mehr kommen. Deswegen verhindern wir das Try
      KollektorGesperrtBisZp = UhrZp / 86400 * 86400 + 115200;                  // (Timestamp / 86400(Sekunden pro Tag)) gerundeter  * 86400(Sekunden pro Tag) = Tagesanfang + 24h = morgen + 8h = morgen um 8 ist Kollektor wieder freigegeben
      Status = KollektorStatus_Sperrzeit;
}    else {
      millis1 = millis();
      Status = KollektorStatus_AUS;
    }
  }
  if ( setValue( _KollektorStatus, Status) && kollLogLevel > 0) {
    Debug.print( F("Koll: Status Zugewiesen: "));           
    Debug.println(Status); 
  }
}











