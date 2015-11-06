#define ThermeVorlaufTempVorgabePin  4
#define ThermeVentilSperrenPin      24
#define ThermeExternAnfordernPin    26
#define ThermeVentilPin             28
#define ThermeVentilVorlaufPin      29
#define ThermeHappyTempPin          30

#define SteuerungStatusNotfallBetrieb_110 -110
#define PumpennachlaufStart_100           -100
#define PumpennachlaufBrenner_90           - 90
#define Pumpennachlauf_80                  - 80
#define VentilAufHKWechseln_70             - 70
#define WarteBisOptoAufWW_60               - 60
#define WarteAufVentilInPossitionHK_50     - 50
#define AnforderungGesperrt             - 40
#define WarteAufAnforderung             - 30
#define WarteAufVentilInPossitionWW     - 20
#define VentilInPossitionWWSperren      - 10


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
  if (Values[_ThermeVorlaufValue].ValueX10 > SteuerungStatusNotfallBetrieb_110) {
    Set_ThermeVorlaufValue(PumpennachlaufStart_100); 
    if (thermeLogLevel > 0) Debug.println ("Therme: Brenner geht aus ->PumpennachlaufStart_100");      
  }
}

void ThermeSolarbetriebBeginnt() {
  digitalWrite(ThermeHappyTempPin, HIGH);
  vitoAngeforderteBetriebsart = ThermeBetriebsartAbschalten;
  Set_ThermeVorlaufValue( WarteAufAnforderung);
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
  Set_ThermeVorlaufValue( SteuerungStatusNotfallBetrieb_110);
  if (thermeLogLevel > 0) Debug.println ("Therme: SteuerungStatusNotfallBetrieb_110");      
}
#define ThermeUmschaltventilWW 30
#define ThermeUmschaltventilHK 10


void   ThermeDoEvents() {
  ThermeUmschaltventilTasterAbfragen();
  BerechneThermeVorlaufValue();
  BerechneThermeVentilVorlaufPin();
}

Bounce thermeVentilBouncer = Bounce(ThermeVentilPin, 500, true); 
void ThermeUmschaltventilTasterAbfragen(){
  if (thermeVentilBouncer.update()) {
    if (thermeVentilBouncer.read()) 
      ValueX10new1 = ThermeUmschaltventilWW; 
    else 
      ValueX10new1 = ThermeUmschaltventilHK;
    if (setValue(_ThermeUmschaltventilTaster, ValueX10new1) && thermeLogLevel > 1) {
      Debug.print   ( F("Therme: ThermeUmschaltventilTaster = "));
      Debug.print   ( Values[ _ThermeUmschaltventilTaster].ValueX10);
      Debug.println ( F( "Zugewiesen"));
    }
  }
}

void Set_ThermeVorlaufValue (int16_t value) {
  if ( setValue( _ThermeVorlaufValue, value) && thermeLogLevel > 1) {
    Debug.print  ( F("Therme "));
    Debug.print  ( millis());
    Debug.print   ( F(": ThermeVorlaufValue Zugewiesen: "));
    Debug.println   ( Values[_ThermeVorlaufValue].ValueX10);
  }
}

void BerechneThermeVorlaufValue() {
  static uint32_t warteSeit1 = 0;

  ValueX10new1 = Values[_ThermeVorlaufValue].ValueX10;
  switch ( ValueX10new1) { 
  case SteuerungStatusNotfallBetrieb_110:
    break;
  case PumpennachlaufStart_100:
    analogWrite( ThermeVorlaufTempVorgabePin, 0);
    ValueX10new1 = PumpennachlaufBrenner_90;
    if (thermeLogLevel > 1) {
      Debug.print  ( F("Therme "));
      Debug.print  ( millis());
      Debug.println( F(": analogWrite( ThermeVorlaufTempVorgabePin, 0); PumpennachlaufBrenner_90"));
    }
    break;
  case PumpennachlaufBrenner_90:
    if ( Values[_ThermeBrennerLeistung].ValueX10 == 0) {
      ValueX10new1 = Pumpennachlauf_80;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": Pumpennachlauf_80"));
      }
    }
    break;
  case Pumpennachlauf_80:
    if ( Values[ _ThermeVorlaufTempIst].ValueX10 < Values[_ThermeRuecklaufTempIst].ValueX10 + 30) {
      warteSeit1 = millis();
      ValueX10new1 = VentilAufHKWechseln_70;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> VentilAufHKWechseln_70"));
      }
    }
    break;
  case VentilAufHKWechseln_70:
    if (Values[_ThermeUmschaltventilTaster].ValueX10 == ThermeUmschaltventilWW) {
      digitalWrite( ThermeExternAnfordernPin, LOW);                             // Ohne Anforderung Wechselt die Therme auf WW
      ValueX10new1 = WarteBisOptoAufWW_60;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> WarteBisOptoAufWW_60"));
      }
    } else {
      ValueX10new1 = WarteAufVentilInPossitionHK_50;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> WarteAufVentilInPossitionHK_50"));
      }
    }
    warteSeit1 = millis();
    break;
  case WarteBisOptoAufWW_60:
    if (Values[_ThermeUmschaltventilOpto].ValueX10 == ThermeUmschaltventilWW) {
      digitalWrite( ThermeVentilSperrenPin, LOW);                                         // das Ventil sperren
      digitalWrite( ThermeExternAnfordernPin, HIGH);
      warteSeit1 = millis();
      ValueX10new1 = WarteAufVentilInPossitionHK_50;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> WarteAufVentilInPossitionHK_50"));
      }
    } else if (millis() - warteSeit1 > 60000) { 
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": TimeOut -> WarteAufAnforderung"));
      }
    }
    break;
  case WarteAufVentilInPossitionHK_50:
    if (   Values[_ThermeUmschaltventilTaster].ValueX10 == ThermeUmschaltventilHK
        && millis() - warteSeit1 > 2000) {
      digitalWrite( ThermeVentilSperrenPin, HIGH);                                         // das Ventil sperren
      digitalWrite( ThermeExternAnfordernPin, LOW);
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> VentilstatusAufWWSyncronisieren"));
      }
    } else if (millis() - warteSeit1 > 60000) { 
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": TimeOut -> WarteAufAnforderung"));
      }
    }
    break;
  case AnforderungGesperrt:
    if (millis() - warteSeit1 > 120000) {
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> WarteAufAnforderung"));
      }
    }
    break;
  case WarteAufAnforderung:
    if (   Values[_HKAnforderung].ValueX10 >= AnforderungTRUE 
        || Values[_WWAnforderung].ValueX10 >= AnforderungTRUE) {
      warteSeit1 = millis();
      ValueX10new1 = WarteAufVentilInPossitionWW;    
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> WarteAufVentilInPossitionWW"));
      }
    } else if (Values[_ThermeUmschaltventilTaster].ValueX10 == ThermeUmschaltventilWW) {
        warteSeit1 = millis();
        ValueX10new1 = VentilAufHKWechseln_70;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": -> VentilAufHKWechseln_70"));
      }
    }
    break;
  case WarteAufVentilInPossitionWW:
    if (Values[_ThermeUmschaltventilTaster].ValueX10 == ThermeUmschaltventilWW) {
      warteSeit1 = millis();
      ValueX10new1 = VentilInPossitionWWSperren;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F(": Ventil passt, in 2 Sek sperren"));
      }
    } else if( millis() - warteSeit1 > 2000) {
      ValueX10new1 = WarteAufAnforderung;
      if (thermeLogLevel > 1) {
        Debug.print  ( F("Therme "));
        Debug.print  ( millis());
        Debug.println( F("Ventil passt noch nicht ->WarteAufAnforderung"));
      }
    }
    break;
  case VentilInPossitionWWSperren:
    if (Values[_ThermeUmschaltventilTaster].ValueX10 == ThermeUmschaltventilWW) {
      if( millis() - warteSeit1 > 2000) {                                                    // scheint stabil zu sein
        digitalWrite( ThermeVentilSperrenPin, HIGH);                                         // das Ventil sperren
//        vitoAngeforderteBetriebsart = ThermeBetriebsartAbschalten;
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme "));
          Debug.print  ( millis());
          Debug.println( F("ThermeVentilSperrenPin = HIGH"));
        }
        //In diesem Zweig KEIN break, damit der default den neuen ValueX10new1 berechnet
      } else {
        break;
      }
    } else {
      ValueX10new1 = WarteAufAnforderung;
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme "));
          Debug.print  ( millis());
          Debug.println( F("Ventil nicht stabil ->WarteAufAnforderung"));
        }
      break;
    }
  default:
    tmpUint16_1 = 0;
    if (Values[_HKAnforderung].ValueX10 >= AnforderungTRUE) {
      tmpUint16_1 = Values[ _HKVorlaufTempSoll].ValueX10 + HKHysterese + HKSpreizung * 2;
    }
    if (Values[_WWAnforderung].ValueX10 >= AnforderungTRUE) {
      tmpUint16_1 = max( tmpUint16_1, Values[ _WWSpeicherTempSoll].ValueX10 + WWHysterese + WWSpreizung + max( 0, Values[ _WWSpeicherTempSoll].ValueX10 - Values[ _WWSpeicherTemp1].ValueX10) * 2);
      //                 Wert von HK übernehmen falls größer
      //                              Temp muß hoch genug sein, also Soll + Hysterese + Spreizung + (Delta T * 2 aber nur wenn größer 0 - damit Heizen wir mehr wenn großer Verbrauch die Temp. stärker drückt)
    }
    if ( tmpUint16_1 > 1036 / 7) {//damit nach der Formel min. 1 über den Statuswerten bleibt
      tmpUint16_1 = max( tmpUint16_1,  Values[_ThermeKesselTempIst].ValueX10 - 25);
      //ValueX10new1 = ThermeVorlaufTempVorgabe;
      //ValueX10new1 = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe - Values[_ThermeVorlaufTempIst].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
      ValueX10new1 = (tmpUint16_1 * 7 - 1036) / 3;//bei 1036 und 65C Soll entstehen 63
      if ( Values[_ThermeVorlaufValue].ValueX10 != ValueX10new1 ) {
        digitalWrite( ThermeExternAnfordernPin, HIGH);
        analogWrite( ThermeVorlaufTempVorgabePin, ValueX10new1 / 10);               // PWM setzen
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme "));
          Debug.print  ( millis());
          Debug.println( F(": B"));
        }
      } 
    }
    else { 
      if (Values[_ThermeVorlaufValue].ValueX10 > 0) {
        ValueX10new1 = PumpennachlaufStart_100;
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme "));
          Debug.print  ( millis());
          Debug.println( F(": A"));
        }
      }
    } 
  }
  Set_ThermeVorlaufValue(ValueX10new1);
}

void BerechneThermeVentilVorlaufPin(){
  if ( digitalRead( ThermeVentilVorlaufPin) == LOW) {
    if ( Values[_ThermeVorlaufValue].ValueX10 >= WarteAufAnforderung
      && Values[ _ThermeVorlaufTempIst].ValueX10 != ValueUnknown
      && Values[ _SpeicherA1          ].ValueX10 != ValueUnknown
      && Values[ _SpeicherA2          ].ValueX10 != ValueUnknown
      && Values[ _ThermeVorlaufTempIst].ValueX10 >= Values[ _SpeicherA1].ValueX10 + 10) {
      digitalWrite( ThermeVentilVorlaufPin, HIGH);
    }
  } 
  else {
    if (    Values[_ThermeVorlaufValue].ValueX10 >= Pumpennachlauf_80
         && Values[_ThermeVorlaufValue].ValueX10 < WarteAufAnforderung) {                     //Wenn Therme gerade abgeschalten hat
      //if ( Values[ _ThermeVorlaufTempIst].ValueX10 <= Values[ _SpeicherA2].ValueX10 + 20) {//erst 2° unter SpeicherA2 runter schalten
        digitalWrite( ThermeVentilVorlaufPin, LOW);
      //}
    }
    else { 
      if ( Values[ _ThermeVorlaufTempIst].ValueX10 <= Values[ _SpeicherA1].ValueX10 - 20) { //normal bei 2° unter A1 runter schalten
        digitalWrite( ThermeVentilVorlaufPin, LOW); 
      } 
    }
  }
  if ( setValue( _ThermeVorlaufVentil, (digitalRead( ThermeVentilVorlaufPin) == HIGH)? 10: 0) && thermeLogLevel > 1) {
    Debug.print   ( F("Therme: ThermeVorlaufVentil Zugewiesen: "));
    Debug.println   ( Values[_ThermeVorlaufVentil].ValueX10);
  }
}























