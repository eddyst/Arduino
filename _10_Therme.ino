#define ThermeVorlaufTempVorgabePin  4
#define ThermeVentilSperrenPin      24
#define ThermeExternAnfordernPin    26
#define ThermeVentilPin             28
#define ThermeVentilVorlaufPin      29
#define ThermeHappyTempPin          30

#define ThermeStatusNotfallBetrieb                      0
#define ThermeStatusSolarBetriebInit                    1
#define ThermeStatusSolarBetriebWarteAufVentil          2
#define ThermeStatusSolarBetriebWarteAufWWbetrieb       3
#define ThermeStatusSolarBetriebWarteAufAbschalten      4
#define ThermeStatusSolarBetriebAktualisieren           5
#define ThermeStatusSolarBetrieb                        6
#define ThermeStatusSolarBetriebPumpennachlaufStart     7
#define ThermeStatusSolarBetriebPumpennachlauf          8
uint8_t ThermeStatus = 0;


#include <Bounce.h>

void ThermeInit() {
  pinMode     (ThermeHappyTempPin         , OUTPUT);
  digitalWrite(ThermeHappyTempPin         , LOW   );

  pinMode     (ThermeVentilPin            , INPUT ); 

  pinMode     (ThermeVentilSperrenPin     , OUTPUT);
  digitalWrite(ThermeVentilSperrenPin     , LOW   );

  pinMode     (ThermeExternAnfordernPin   , OUTPUT);
  digitalWrite(ThermeExternAnfordernPin   , LOW   );

  pinMode     (ThermeVorlaufTempVorgabePin, OUTPUT);
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
}

void ThermeVorlaufTempVorgabeRechnen() {
  if (ThermeStatus == ThermeStatusSolarBetrieb) {
    ThermeStatus = ThermeStatusSolarBetriebAktualisieren;
    if (thermeLogLevel > 0) Debug.println ("Therme: ThermeStatusSolarBetriebAktualisieren");      
  }
}

void ThermeBrennerGehtAus() {
  if (ThermeStatus > ThermeStatusSolarBetriebWarteAufVentil) {
    ThermeStatus = ThermeStatusSolarBetriebPumpennachlaufStart;
    if (thermeLogLevel > 0) Debug.println ("Therme: ThermeStatusSolarBetriebPumpennachlaufStart");      
  }
}

void ThermeSolarbetriebBeginnt() {
  digitalWrite(ThermeHappyTempPin, HIGH);
  ThermeStatus = ThermeStatusSolarBetriebInit;
  if (thermeLogLevel > 0) {
    Debug.println ( F("Therme: ThermeStatusSolarBetriebInit"));
    Debug.println ( F("        HappyTempPin = HIGH"));
  }
}

void ThermeSolarbetriebEndet () {
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
  digitalWrite(ThermeExternAnfordernPin   , LOW   );
  digitalWrite(ThermeVentilSperrenPin     , LOW   );
  digitalWrite(ThermeHappyTempPin         , LOW   );
  ThermeStatus = ThermeStatusNotfallBetrieb;
  if (thermeLogLevel > 0) Debug.println ("Therme: ThermeStatusNotfallBetrieb");      
}

void   ThermeDoEvents() {
  static uint32_t warteSeit1 = 0, warteSeit2 = 0;
  static Bounce bouncer = Bounce(ThermeVentilPin, 500); 
  if (bouncer.update()) {
    uint8_t newValueX10;
    if (bouncer.read()) 
      newValueX10 = 30; 
    else 
      newValueX10 = 10;
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
      ThermeStatus = ThermeStatusSolarBetriebWarteAufVentil;       // und Status  zurücksetzen
      if (thermeLogLevel > 0) Debug.println ( F("Therme: ThermeStatusSolarBetriebWarteAufVentil"));
    }
    warteSeit2 = millis();
  }
  if (Solarbetrieb) {
    if (ThermeStatus < ThermeStatusSolarBetriebAktualisieren && millis() - warteSeit1 > 120000) { //Init hat nicht funktioniert --> nochmal
      ThermeSolarbetriebBeginnt();
    }
    uint8_t  ThermeVorlaufTempVorgabe = 0; 

    switch (ThermeStatus) {                        // Ventil steht richtig
    case ThermeStatusSolarBetrieb:
      if ( millis() - warteSeit1 > 30000) { // Nach ? Sek mal neu rechnen auch wenn sich keine Temperaturen geändert haben
        ThermeVorlaufTempVorgabeRechnen();
      }
      break;

    case ThermeStatusSolarBetriebAktualisieren:                                          // ThermeVorlaufTempSollX10 durch anpassung der PWM gemäß ThermeVorlaufTempVorgabe nachführen   
      static uint8_t  ThermeVorlaufTempVorgabeValue = 0;       
      if (Values[_HKAnforderung].ValueX10 >= AnforderungTRUE) {
        ThermeVorlaufTempVorgabe = HKSollTempVorgabe + HKHysterese + 5;
      }
      if (Values[_WWAnforderung].ValueX10 >= AnforderungTRUE
        && ThermeVorlaufTempVorgabe < WWSollTempVorgabe + WWHysterese + 5) {
        ThermeVorlaufTempVorgabe = WWSollTempVorgabe + WWHysterese + 5;
      }

      if (thermeLogLevel > 1){ 
        Debug.print  ( F("Therme: ThermeVorlaufTempVorgabe = "));
        Debug.println( ThermeVorlaufTempVorgabe );
      }
      if ( ThermeVorlaufTempVorgabe > 0) {
        digitalWrite( ThermeExternAnfordernPin, HIGH);
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
        ThermeStatus = ThermeStatusSolarBetriebPumpennachlaufStart;
        if (thermeLogLevel > 1) {
          Debug.println ( F("Therme: ThermeStatusSolarBetriebPumpennachlaufStart"));
        }
      } 
      warteSeit1 = millis();                                                           // Wartezeit initialisieren
      ThermeStatus = ThermeStatusSolarBetrieb;                             // und in ThermeStatusSolarBetrieb wechseln
      break;
    case ThermeStatusSolarBetriebPumpennachlaufStart:
      analogWrite( ThermeVorlaufTempVorgabePin, 0);
      if (thermeLogLevel > 0) Debug.println ( F("Therme: analogWrite( ThermeVorlaufTempVorgabePin, 0);"));
      warteSeit2 = millis();
      ThermeStatus = ThermeStatusSolarBetriebPumpennachlauf;
      break;
    case ThermeStatusSolarBetriebPumpennachlauf:
      if ( millis() - warteSeit2 > 120000) { // Pumpennachlauf beenden
        digitalWrite( ThermeExternAnfordernPin, LOW);
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme: millis = "));
          Debug.println(millis());
          Debug.println( F("    ExternAnfordern = LOW"));
        }
        ThermeStatus = ThermeStatusSolarBetriebAktualisieren;
      }
      break;

    case ThermeStatusSolarBetriebInit:
      warteSeit1 = millis(); 
      ThermeStatus = ThermeStatusSolarBetriebWarteAufVentil;
      if (thermeLogLevel > 1) {
        Debug.println ( F("Therme: ThermeStatusSolarBetriebWarteAufVentil"));
      }
      break;

    case ThermeStatusSolarBetriebWarteAufVentil:
      if (bouncer.read() == HIGH) {                                                            // Na wird doch langsam
        if( millis() - warteSeit2 > 2000) {                                                    // scheint stabil zu sein
          digitalWrite( ThermeVentilSperrenPin, HIGH);                                         // das Ventil sperren
          warteSeit2 = millis();
          vitoAngeforderteBetriebsart = ThermeBetriebsartAbschalten;
          ThermeStatus = ThermeStatusSolarBetriebWarteAufAbschalten;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeVentilSperrenPin = HIGH"));
            Debug.println ( F("        ThermeStatusSolarBetriebWarteAufAbschalten"));
          }
        }
      } 
      else { //Vielleicht hilft ja den Modus mal auf WW zu schalten?
        warteSeit2 = millis();
        vitoAngeforderteBetriebsart = ThermeBetriebsartWW;
        ThermeStatus = ThermeStatusSolarBetriebWarteAufWWbetrieb;
        if (thermeLogLevel > 1) {
          Debug.println ( F("Therme: ThermeStatusSolarBetriebWarteAufWWbetrieb (10Sec)"));
        }
      }
      break;
    case ThermeStatusSolarBetriebWarteAufWWbetrieb:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartWW * 10){                         //Thermenstatus = WW - so muß nun mal aus hydraulischen Grnden das Ventil stehen
          ThermeStatus = ThermeStatusSolarBetriebWarteAufVentil;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeStatusSolarBetriebWarteAufVentil (10Sec)"));
          }
        }
      }
      break;
    case ThermeStatusSolarBetriebWarteAufAbschalten:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartAbschalten * 10){                //Thermenstatus = Heizen - sonst bekomme ich keine VorlaufTempSoll
          ThermeStatus = ThermeStatusSolarBetriebAktualisieren;
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeStatusSolarBetriebAktualisieren"));
          }
        }      
      }
      break;
    }
  }
}























