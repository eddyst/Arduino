#define HappyTempPin                24
#define ThermeVentilPin             26
#define ThermeVentilSperrenPin      28
#define ExternAnfordernPin          30
#define ThermeVorlaufTempVorgabePin  4

#define ThermeControlStateSolarBetriebInit                    1
#define ThermeControlStateSolarBetriebWarteAufVentil          2
#define ThermeControlStateSolarBetriebWarteAufWWbetrieb       3
#define ThermeControlStateSolarBetriebWarteAufHeizbetrieb     4
#define ThermeControlStateSolarBetriebAktualisieren           5
#define ThermeControlStateSolarBetrieb                        6
#define ThermeControlStateSolarBetriebPumpennachlaufStart     7
#define ThermeControlStateSolarBetriebPumpennachlauf          8
uint8_t ThermeControlState = 0;

#define AktivNOT_INITIALIZED            0
#define AktivFALSE_Temp1_UNKNOWN       10
#define AktivFALSE_Temp1               20
#define AktivFALSE_Temp2_UNKNOWN       30
#define AktivFALSE_Temp2               40
#define AktivFALSE_Temp3_UNKNOWN       50
#define AktivFALSE_Temp3               60
#define AktivFALSE_Zeitlimit           70
#define AktivFALSE_Ausschaltkriterien  80
#define AktivTRUE                     100
#define AktivTRUE_Temp2               110
#define AktivTRUE_Temp3               120

#include <Bounce.h>

void ThermeInit() {
  pinMode     (HappyTempPin               , OUTPUT);
  digitalWrite(HappyTempPin               , LOW   );

  pinMode     (ThermeVentilPin            , INPUT ); 

  pinMode     (ThermeVentilSperrenPin     , OUTPUT);
  digitalWrite(ThermeVentilSperrenPin     , LOW   );

  pinMode     (ExternAnfordernPin         , OUTPUT);
  digitalWrite(ExternAnfordernPin         , LOW   );

  pinMode     (ThermeVorlaufTempVorgabePin, OUTPUT);
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
}

void ThermeVorlaufTempVorgabeRechnen() {
  if (ThermeControlState == ThermeControlStateSolarBetrieb) {
    ThermeControlState = ThermeControlStateSolarBetriebAktualisieren;
    if (thermeLogLevel > 0) Debug.println ("Therme: ThermeControlStateSolarBetriebAktualisieren");      
  }
}

void ThermeBrennerGehtAus() {
  if (ThermeControlState > ThermeControlStateSolarBetriebWarteAufVentil) {
    ThermeControlState = ThermeControlStateSolarBetriebPumpennachlaufStart;
    if (thermeLogLevel > 0) Debug.println ("Therme: ThermeControlStateSolarBetriebPumpennachlaufStart");      
  }
}

void ThermeSolarbetriebBeginnt() {
  digitalWrite(HappyTempPin, HIGH);
  ThermeControlState = ThermeControlStateSolarBetriebInit;
  if (thermeLogLevel > 0) {
    Debug.println ( F("Therme: ThermeControlStateSolarBetriebInit"));
    Debug.println ( F("        HappyTempPin = HIGH"));
  }
}

void ThermeSolarbetriebEndet () {
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
  digitalWrite(ExternAnfordernPin         , LOW   );
  digitalWrite(ThermeVentilSperrenPin     , LOW   );
  digitalWrite(HappyTempPin               , LOW   );
  if (thermeLogLevel > 0) Debug.println ("Therme: ThermeControlStateNotfallBetrieb");      
}

uint8_t HKAnforderung() {      
  uint8_t  Anforderung = AktivNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if (Values[_HKAnforderung].ValueX10 < AktivTRUE * 10) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_HKVorlaufTemp1].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp1_UNKNOWN;
    else if ( Values[_HKVorlaufTemp1].ValueX10 >= HKSollTempVorgabe * 10) Anforderung = AktivFALSE_Temp1;
    else if ( Values[_SpeicherA2    ].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA2    ].ValueX10 >= HKSollTempVorgabe * 10) Anforderung = AktivFALSE_Temp2; 
    else if ( Values[_SpeicherA3    ].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp3_UNKNOWN;
    else if ( millis() - AnforderungSeit > 300000 )                       Anforderung = AktivFALSE_Zeitlimit; 
    else                                                                  Anforderung = AktivTRUE;
    if ( Anforderung < AktivFALSE_Zeitlimit){ // Die Einschaltkriterien sind nicht erfüllt
      AnforderungSeit = millis();      //Wartezeit  zurücksetzen
    }
  } 
  else {                                           //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1].ValueX10 < ( HKSollTempVorgabe + HKHysterese) * 10) Anforderung = AktivTRUE_Temp2;
    else if ( Values[_SpeicherA2].ValueX10 < ( HKSollTempVorgabe + HKHysterese) * 10) Anforderung = AktivTRUE_Temp3;
    else                                                                              Anforderung = AktivFALSE_Ausschaltkriterien;
  }
  if ( Values[_HKAnforderung].ValueX10 != Anforderung * 10) { //Is anders als vorhin -> dokumentieren
    Values[_HKAnforderung].ValueX10 = Anforderung * 10;
    Values[_HKAnforderung].Changed = 1;
    if (thermeLogLevel > 1) {
      Debug.print   ( F("Therme: HKAnforderung = "));
      Debug.print   ( Values[_HKAnforderung].ValueX10);
      Debug.println ( F( "Zugewiesen"));
    }
  }
  return Anforderung;
}

uint8_t WWAnforderung() {      
  uint8_t  Anforderung = AktivNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if ( WWSollTempVorgabe > 60 - WWHysterese)
    WWSollTempVorgabe = 60 - WWHysterese;
  if (Values[_WWAnforderung].ValueX10 < AktivTRUE * 10) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_WWSpeicherTemp1].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp1_UNKNOWN;
    else if ( Values[_WWSpeicherTemp1].ValueX10 >= WWSollTempVorgabe * 10) Anforderung = AktivFALSE_Temp1;
    else if ( Values[_SpeicherA1     ].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA1     ].ValueX10 >= WWSollTempVorgabe * 10) Anforderung = AktivFALSE_Temp2; 
    else if ( Values[_SpeicherA2     ].ValueX10 == ValueUnknown)           Anforderung = AktivFALSE_Temp3_UNKNOWN;
    else if ( millis() - AnforderungSeit > 30000 )                         Anforderung = AktivFALSE_Zeitlimit; 
    else                                                                   Anforderung = AktivTRUE;
    if ( Anforderung < AktivFALSE_Zeitlimit){ // Die Einschaltkriterien sind nicht erfüllt
      AnforderungSeit = millis();      //Wartezeit  zurücksetzen
    }
  } 
  else {                                           //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1     ].ValueX10 > ( WWSollTempVorgabe + WWHysterese) * 10) Anforderung = AktivTRUE_Temp2;
    else if ( Values[_SpeicherA2     ].ValueX10 > ( WWSollTempVorgabe + WWHysterese) * 10) Anforderung = AktivTRUE_Temp3;
    else                                                                                   Anforderung = AktivFALSE_Ausschaltkriterien;
  }
  if ( Values[_WWAnforderung].ValueX10 != Anforderung * 10) { //Is anders als vorhin -> dokumentieren
    Values[_WWAnforderung].ValueX10 = Anforderung * 10;
    Values[_WWAnforderung].Changed = 1;
    if (thermeLogLevel > 1) {
      Debug.print   ( F("Therme: WWAnforderung = "));
      Debug.print   ( Values[_WWAnforderung].ValueX10);
      Debug.println ( F( "Zugewiesen"));
    }
  }
  return Anforderung;
}

void   ThermeDoEvents() {
  static uint32_t warteSeit1 = 0, warteSeit2 = 0;
  static Bounce bouncer = Bounce(ThermeVentilPin, 500); 
  if (bouncer.update()) {
    uint8_t newValueX10;
    if (bouncer.read()) newValueX10 = 30; 
    else newValueX10 = 10;
    if (Values[_ThermeUmschaltventilTaster].ValueX10 != newValueX10) { //Erstmal dokumentieren
      Values[_ThermeUmschaltventilTaster].ValueX10 = newValueX10;
      Values[_ThermeUmschaltventilTaster].Changed = 1;
      if (thermeLogLevel > 1) {
        Debug.print   ( F("Therme: ThermeUmschaltventilTaster = "));
        Debug.print   ( newValueX10);
        Debug.println ( F( "Zugewiesen"));
      }
    }
    if (Solarbetrieb && bouncer.fallingEdge()) {                               // Wir sind nicht mehr im richtigen Modus
      digitalWrite( ThermeVentilSperrenPin, LOW);                              // das Ventil freigeben
      ThermeControlState = ThermeControlStateSolarBetriebWarteAufVentil;       // und Status  zurücksetzen
      if (thermeLogLevel > 0) Debug.println ( F("Therme: ThermeControlStateSolarBetriebWarteAufVentil"));
    } 
  }
  if (Solarbetrieb) {
    if (ThermeControlState < ThermeControlStateSolarBetriebAktualisieren && millis() - warteSeit1 > 120000) { //Init hat nicht funktioniert --> nochmal
      ThermeSolarbetriebBeginnt();
    }
    uint8_t  ThermeVorlaufTempVorgabe = 0; 

    switch (ThermeControlState) {                        // Ventil steht richtig

    case ThermeControlStateSolarBetrieb:
      if ( millis() - warteSeit1 > 30000) { // Nach ? Sek mal neu rechnen auch wenn sich keine Temperaturen geändert haben
        ThermeVorlaufTempVorgabeRechnen();
      }
      break;

    case ThermeControlStateSolarBetriebAktualisieren:                                          // ThermeVorlaufTempSollX10 durch anpassung der PWM gemäß ThermeVorlaufTempVorgabe nachführen   
      static uint8_t  ThermeVorlaufTempVorgabeValue = 0;       
      if (HKAnforderung() >= AktivTRUE) {
        ThermeVorlaufTempVorgabe = HKSollTempVorgabe + HKHysterese + 5;
      }
      if (WWAnforderung() >= AktivTRUE
        && ThermeVorlaufTempVorgabe < WWSollTempVorgabe + WWHysterese + 5) {
        ThermeVorlaufTempVorgabe = WWSollTempVorgabe + WWHysterese + 5;
      }

      if (thermeLogLevel > 1){ 
        Debug.print  ( F("Therme: ThermeVorlaufTempVorgabe = "));
        Debug.println( ThermeVorlaufTempVorgabe );
      }
      if ( ThermeVorlaufTempVorgabe > 0) {
        digitalWrite( ExternAnfordernPin, HIGH);
        //ThermeVorlaufTempVorgabeValue = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe * 10 - Values[_ThermeVorlaufTempSoll].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
        ThermeVorlaufTempVorgabeValue = (ThermeVorlaufTempVorgabe * 600 - 4200) / 256;

        analogWrite( ThermeVorlaufTempVorgabePin, ThermeVorlaufTempVorgabeValue);               // PWM setzen
        if (thermeLogLevel > 1) {
          Debug.println( F("    ExternAnfordern = High"));
          Debug.print( F("    Values[_ThermeVorlaufTempSoll].ValueX10 = "));
          Debug.println( Values[_ThermeVorlaufTempSoll].ValueX10);
          Debug.print  ( F("    ThermeVorlaufTempVorgabeValue = "));
          Debug.println( ThermeVorlaufTempVorgabeValue);
        }
      } 
      else{ 
        ThermeControlState = ThermeControlStateSolarBetriebPumpennachlaufStart;
        if (thermeLogLevel > 1) {
          Debug.println ( F("Therme: ThermeControlStateSolarBetriebPumpennachlaufStart"));
        }
      } 
      warteSeit1 = millis();                                                           // Wartezeit initialisieren
      ThermeControlState = ThermeControlStateSolarBetrieb;                             // und in ThermeControlStateSolarBetrieb wechseln
      break;
    case ThermeControlStateSolarBetriebPumpennachlaufStart:
      analogWrite( ThermeVorlaufTempVorgabePin, 0);
      if (thermeLogLevel > 0) Debug.println ( F("Therme: analogWrite( ThermeVorlaufTempVorgabePin, 0);"));
      warteSeit2 = millis();
      ThermeControlState = ThermeControlStateSolarBetriebPumpennachlauf;
      break;
    case ThermeControlStateSolarBetriebPumpennachlauf:
      if ( millis() - warteSeit2 > 120000) { // Pumpennachlauf beenden
        digitalWrite( ExternAnfordernPin, LOW);
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme: millis = "));
          Debug.println(millis());
          Debug.println( F("    ExternAnfordern = LOW"));
        }
        ThermeControlState = ThermeControlStateSolarBetriebAktualisieren;
      }
      break;

    case ThermeControlStateSolarBetriebInit:
      warteSeit1 = millis(); 
      ThermeControlState = ThermeControlStateSolarBetriebWarteAufVentil;
      if (thermeLogLevel > 1) {
        Debug.println ( F("Therme: ThermeControlStateSolarBetriebWarteAufVentil"));
      }
      break;

    case ThermeControlStateSolarBetriebWarteAufVentil:
      if (bouncer.read() == HIGH) {                                                            // Na wird doch langsam
        if( bouncer.duration() > 2000) {                                                       // scheint stabil zu sein
          digitalWrite( ThermeVentilSperrenPin, HIGH);                                         // das Ventil sperren
          warteSeit2 = millis();
          vitoAngeforderteBetriebsart = ThermeBetriebsartHeizenUndWW;
          ThermeControlState = ThermeControlStateSolarBetriebWarteAufHeizbetrieb;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeVentilSperrenPin = HIGH"));
            Debug.println ( F("        ThermeControlStateSolarBetriebWarteAufHeizbetrieb"));
          }
        }
      } 
      else { //Vielleicht hilft ja den Modus mal auf WW zu schalten?
        warteSeit2 = millis();
        vitoAngeforderteBetriebsart = ThermeBetriebsartWW;
        ThermeControlState = ThermeControlStateSolarBetriebWarteAufWWbetrieb;
        if (thermeLogLevel > 1) {
          Debug.println ( F("Therme: ThermeControlStateSolarBetriebWarteAufWWbetrieb (10Sec)"));
        }
      }
      break;
    case ThermeControlStateSolarBetriebWarteAufWWbetrieb:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartWW * 10){                         //Thermenstatus = WW - so muß nun mal aus hydraulischen Grnden das Ventil stehen
          ThermeControlState = ThermeControlStateSolarBetriebWarteAufVentil;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeControlStateSolarBetriebWarteAufVentil (10Sec)"));
          }
        }
      }
      break;
    case ThermeControlStateSolarBetriebWarteAufHeizbetrieb:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartHeizenUndWW * 10){                //Thermenstatus = Heizen - sonst bekomme ich keine VorlaufTempSoll
          ThermeControlState = ThermeControlStateSolarBetriebAktualisieren;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeControlStateSolarBetriebAktualisieren"));
          }
        }      
      }
      break;
    }
  }
}























