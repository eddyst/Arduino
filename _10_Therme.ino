#define ThermeVorlaufTempVorgabePin  4
#define ThermeVentilSperrenPin      24
#define ThermeExternAnfordernPin    26
#define ThermeVentilPin             28
#define ThermeVentilVorlaufPin      29
#define ThermeHappyTempPin          30

#define SteuerungStatusNotfallBetrieb                       0
#define SteuerungStatusSolarBetriebInit                    10
#define SteuerungStatusSolarBetriebWarteAufVentil          20
#define SteuerungStatusSolarBetriebWarteAufWWbetrieb       30
#define SteuerungStatusSolarBetriebWarteAufAUSbetrieb      40
#define SteuerungStatusSolarBetrieb                        60

#define PumpennachlaufStart -30
#define Pumpennachlauf      -20
#define WarteAufAnforderung   0

#include <Bounce.h>

void ThermeInit() {
  pinMode     (ThermeHappyTempPin         , OUTPUT);
  digitalWrite(ThermeHappyTempPin         , LOW   );

  pinMode     (ThermeVentilPin            , INPUT ); 

  pinMode     (ThermeVentilSperrenPin     , OUTPUT);
  digitalWrite(ThermeVentilSperrenPin     , LOW   );

  pinMode     (ThermeVentilVorlaufPin     , OUTPUT);
  digitalWrite(ThermeVentilVorlaufPin     , LOW   );

  pinMode     (ThermeExternAnfordernPin   , OUTPUT);
  digitalWrite(ThermeExternAnfordernPin   , LOW   );

  pinMode     (ThermeVorlaufTempVorgabePin, OUTPUT);
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
}

void ThermeBrennerGehtAus() {
  if (Values[_SteuerungStatus].ValueX10 > SteuerungStatusSolarBetriebWarteAufVentil) {
    Values[_ThermeVorlaufValue].ValueX10 = PumpennachlaufStart; //Geändert setzen sparen wir uns
    if (thermeLogLevel > 0) Debug.println ("Therme: SteuerungStatusSolarBetriebPumpennachlaufStart");      
  }
}

void ThermeSolarbetriebBeginnt() {
  digitalWrite(ThermeHappyTempPin, HIGH);
  ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebInit);
  if (thermeLogLevel > 0) {
    Debug.println ( F("Therme: SteuerungStatusSolarBetriebInit"));
    Debug.println ( F("        HappyTempPin = HIGH"));
  }
}

void ThermeSolarbetriebEndet () {
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
  digitalWrite(ThermeExternAnfordernPin   , LOW   );
  digitalWrite(ThermeVentilSperrenPin     , LOW   );
  digitalWrite(ThermeHappyTempPin         , LOW   );
  ThermeSetSteuerungStatus( SteuerungStatusNotfallBetrieb);
  if (thermeLogLevel > 0) Debug.println ("Therme: SteuerungStatusNotfallBetrieb");      
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
    if (Values[_SteuerungStatus].ValueX10 > SteuerungStatusNotfallBetrieb && bouncer.fallingEdge()) {  // Wir sind nicht mehr im richtigen Modus
      digitalWrite( ThermeVentilSperrenPin, LOW);                              // das Ventil freigeben
      ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebWarteAufVentil);                   // und Status  zurücksetzen
      if (thermeLogLevel > 0) Debug.println ( F("Therme: SteuerungStatusSolarBetriebWarteAufVentil"));
    }
    warteSeit2 = millis();
  }
  ///////////////////////////////////////////////////////////////////////////
  if (Values[_SteuerungStatus].ValueX10 > SteuerungStatusNotfallBetrieb) {
    if (Values[_SteuerungStatus].ValueX10 < SteuerungStatusSolarBetrieb && millis() - warteSeit1 > 120000) { //Init hat nicht funktioniert --> nochmal
      ThermeSolarbetriebBeginnt();
    }
    switch (Values[_SteuerungStatus].ValueX10) {                        // Ventil steht richtig
    case SteuerungStatusSolarBetriebInit:
      warteSeit1 = millis(); 
      ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebWarteAufVentil);
      if (thermeLogLevel > 1) {
        Debug.println ( F("Therme: SteuerungStatusSolarBetriebWarteAufVentil"));
      }
      break;

    case SteuerungStatusSolarBetriebWarteAufVentil:
      if (bouncer.read() == HIGH) {                                                            // Na wird doch langsam
        if( millis() - warteSeit2 > 2000) {                                                    // scheint stabil zu sein
          digitalWrite( ThermeVentilSperrenPin, HIGH);                                         // das Ventil sperren
          warteSeit2 = millis();
          vitoAngeforderteBetriebsart = ThermeBetriebsartAbschalten;
          ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebWarteAufAUSbetrieb);
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: ThermeVentilSperrenPin = HIGH"));
            Debug.println ( F("        SteuerungStatusSolarBetriebWarteAufAUSbetrieb"));
          }
        }
      } 
      else { //Vielleicht hilft ja den Modus mal auf WW zu schalten?
        warteSeit2 = millis();
        vitoAngeforderteBetriebsart = ThermeBetriebsartWW;
        ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebWarteAufWWbetrieb);
        if (thermeLogLevel > 1) {
          Debug.println ( F("Therme: SteuerungStatusSolarBetriebWarteAufWWbetrieb (10Sec)"));
        }
      }
      break;
    case SteuerungStatusSolarBetriebWarteAufWWbetrieb:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartWW * 10){                         //Thermenstatus = WW - so muß nun mal aus hydraulischen Grnden das Ventil stehen
          ThermeSetSteuerungStatus( SteuerungStatusSolarBetriebWarteAufVentil);
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: SteuerungStatusSolarBetriebWarteAufVentil (10Sec)"));
          }
        }
      }
      break;
    case SteuerungStatusSolarBetriebWarteAufAUSbetrieb:
      if ( millis() - warteSeit2 > 10000) { // Ventil hatte jetzt auch Zeit sich zu bewegen
        if (Values[_ThermeBetriebsart].ValueX10 == ThermeBetriebsartAbschalten * 10){                //Thermenstatus = Abschalten. Bei externer Anforderung bekomme ich sowieso keine VorlaufTempSoll
          ThermeSetSteuerungStatus( SteuerungStatusSolarBetrieb);
          if (thermeLogLevel > 1) {
            Debug.println ( F("Therme: SteuerungStatusSolarBetriebAktualisieren"));
          }
        }      
      }
      break;
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ValueX10new1 = 0;
  switch ( Values[_ThermeVorlaufValue].ValueX10) { //Pumpennachlauf
  case PumpennachlaufStart:
    analogWrite( ThermeVorlaufTempVorgabePin, 0);
    if (thermeLogLevel > 0) Debug.println ( F("Therme: analogWrite( ThermeVorlaufTempVorgabePin, 0);"));
    warteSeit2 = millis();
    ValueX10new1 = Pumpennachlauf;
    break;
  case Pumpennachlauf:
    if ( millis() - warteSeit2 > 120000) { // Pumpennachlauf beenden
      digitalWrite( ThermeExternAnfordernPin, LOW);
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme: millis = "));
        Debug.println(millis());
        Debug.println( F("    ExternAnfordern = LOW"));
      }
    }
    break;
  default:
    uint16_t  TempSollMax = 0; 
    if (Values[_HKAnforderung].ValueX10 >= AnforderungTRUE) {
      TempSollMax = Values[_HKVorlaufTempSoll].ValueX10 + HKHysterese + HKSpreizung * 2;
    }
    if (Values[_WWAnforderung].ValueX10 >= AnforderungTRUE
      && TempSollMax < Values[_WWSpeicherTempSoll].ValueX10 + WWHysterese + WWSpreizung * 2) {
         TempSollMax = Values[_WWSpeicherTempSoll].ValueX10 + WWHysterese + WWSpreizung * 2;
    }

    if ( TempSollMax > 150) {
      //ValueX10new1 = ThermeVorlaufTempVorgabe;
      //ValueX10new1 = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe - Values[_ThermeVorlaufTempIst].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
      ValueX10new1 = (TempSollMax * 7 - 1050) / 3;
      if ( Values[_ThermeVorlaufValue].ValueX10 != ValueX10new1 ) {
        digitalWrite( ThermeExternAnfordernPin, HIGH);
        analogWrite( ThermeVorlaufTempVorgabePin, ValueX10new1 / 10);               // PWM setzen
      } 
    }
    else { 
      if (Values[_ThermeVorlaufValue].ValueX10 > 0) {
        ValueX10new1 = PumpennachlaufStart;
      }
    } 
  }
  if ( setValue( _ThermeVorlaufValue, ValueX10new1) && thermeLogLevel > 1) {
    Debug.print   ( F("Therme: ThermeVorlaufValue Zugewiesen: "));
    Debug.println   ( Values[_ThermeVorlaufValue].ValueX10);
  }




  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ( digitalRead( ThermeVentilVorlaufPin) == LOW) {
    if ( Values[ _ThermeVorlaufTempIst].ValueX10 != ValueUnknown
      && Values[ _SpeicherA2          ].ValueX10 != ValueUnknown
      && Values[ _ThermeVorlaufTempIst].ValueX10 > Values[ _SpeicherA2].ValueX10 + 5) {
      digitalWrite( ThermeVentilVorlaufPin, HIGH);
    }
  } 
  else {
    if ( Values[ _ThermeVorlaufTempIst].ValueX10 < Values[ _SpeicherA2].ValueX10) {
      digitalWrite( ThermeVentilVorlaufPin, LOW); 
    } 
  }
  if ( setValue( _ThermeVorlaufVentil, iif(digitalRead( ThermeVentilVorlaufPin) == HIGH, 10, 0)) && thermeLogLevel > 1) {
    Debug.print   ( F("Therme: ThermeVorlaufVentil Zugewiesen: "));
    Debug.println   ( Values[_ThermeVorlaufVentil].ValueX10);
  }
}


void ThermeSetSteuerungStatus (int16_t value) {
  if ( setValue( _SteuerungStatus, value) && thermeLogLevel > 1) {
    Debug.print   ( F("Therme: SteuerungStatus Zugewiesen: "));
    Debug.println   ( Values[_SteuerungStatus].ValueX10);
  }
}























