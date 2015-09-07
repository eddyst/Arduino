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

void   ThermeDoEvents() {
  static uint32_t warteSeit1 = 0;
  static Bounce bouncer = Bounce(ThermeVentilPin, 500); 
  if (bouncer.update()) {
    if (Values[_ThermeUmschaltventilTaster].ValueX10 != bouncer.read() * 10) { //Erstmal dokumentieren
      Values[_ThermeUmschaltventilTaster].ValueX10 = bouncer.read() * 10;
      Values[_ThermeUmschaltventilTaster].Changed = 1;
      if (thermeLogLevel > 1) {
        Debug.print   ( F("Therme: ThermeUmschaltventilTaster = "));
        Debug.print   ( bouncer.read() * 10);
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
      static uint32_t HKAnforderungSeit, WWAnforderungSeit, warteSeit2;
      static boolean  HKAktiv = false  , WWAktiv = false  ;
      static uint8_t  ThermeVorlaufTempVorgabeValue = 0;       
      if ( Values[_HKVorlaufTemp].ValueX10 != ValueUnknown
        && Values[_HKVorlaufTemp].ValueX10 < HKSollTempVorgabe * 10
        && Values[_SpeicherA1   ].ValueX10 != ValueUnknown
        && Values[_SpeicherA2   ].ValueX10 != ValueUnknown
        && Values[_SpeicherA2   ].ValueX10 < HKSollTempVorgabe * 10  ) {  //Einschaltkriterium erfüllt
        if (! HKAktiv
          && millis() - HKAnforderungSeit > 300000) {                      //Einschaltwartezeit abgelaufen
          HKAktiv = true;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: HKAktiv = true"));
        }
      } 
      else {                                                              //Einschaltkriterium nicht erfüllt
        HKAnforderungSeit = millis();                                     //Wartezeit zurücksetzen
        if (HKAktiv                                                       
          && Values[_SpeicherA1].ValueX10 > HKSollTempVorgabe * 10 + HKHysterese
          && Values[_SpeicherA2].ValueX10 > HKSollTempVorgabe * 10 + HKHysterese){      //Ausschaltkriterium erfüllt
          HKAktiv = false;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: HKAktiv = false"));
        }
      }
      if (HKAktiv) {
        ThermeVorlaufTempVorgabe = HKSollTempVorgabe + HKHysterese + 5;
      }
      if ( WWSollTempVorgabe > 60 - WWHysterese)
        WWSollTempVorgabe = 60 - WWHysterese;
      if ( Values[_WWSpeicherTemp1].ValueX10 != ValueUnknown
        && Values[_WWSpeicherTemp1].ValueX10 < WWSollTempVorgabe * 10
        && Values[_SpeicherA1     ].ValueX10 != ValueUnknown
        && Values[_SpeicherA1     ].ValueX10 < WWSollTempVorgabe * 10  
        && Values[_SpeicherA2     ].ValueX10 != ValueUnknown ) {           //Einschaltkriterium erfüllt
        if ( ! WWAktiv 
          && millis() - WWAnforderungSeit > 30000) {                       //Einschaltwartezeit abgelaufen
          WWAktiv = true;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: WWAktiv = true"));
        }
      } 
      else{                                                               //Einschaltkriterium nicht erfüllt
        if (WWAktiv
          && (   Values[_WWSpeicherTemp1].ValueX10 > WWSollTempVorgabe * 10 + WWHysterese
          || (   Values[_SpeicherA1     ].ValueX10 > WWSollTempVorgabe * 10 + WWHysterese
          && Values[_SpeicherA2     ].ValueX10 > WWSollTempVorgabe * 10 + WWHysterese
          ))) { //Ausschaltkriterium erfüllt
          WWAktiv = false;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: WWAktiv = false"));
        }
        WWAnforderungSeit = millis();
      }
      if (WWAktiv && ThermeVorlaufTempVorgabe < WWSollTempVorgabe) {
        ThermeVorlaufTempVorgabe = WWSollTempVorgabe + WWHysterese + 5;
      }

      if (thermeLogLevel > 1){ 
        Debug.print  ( F("Therme: ThermeVorlaufTempVorgabe = "));
        Debug.println( ThermeVorlaufTempVorgabe );
      }
      if ( ThermeVorlaufTempVorgabe > 0) {
        digitalWrite( ExternAnfordernPin, HIGH);
        //ThermeVorlaufTempVorgabeValue = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe * 10 - Values[_ThermeVorlaufTempSoll].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
        /* Stufen sind nicht wirklich schön
         if (ThermeVorlaufTempVorgabe < 29) ThermeVorlaufTempVorgabeValue = 50;
         else if (ThermeVorlaufTempVorgabe < 34) ThermeVorlaufTempVorgabeValue = 60;
         else if (ThermeVorlaufTempVorgabe < 39) ThermeVorlaufTempVorgabeValue = 70;
         else if (ThermeVorlaufTempVorgabe < 44) ThermeVorlaufTempVorgabeValue = 80;
         else if (ThermeVorlaufTempVorgabe < 49) ThermeVorlaufTempVorgabeValue = 90;
         else if (ThermeVorlaufTempVorgabe < 53) ThermeVorlaufTempVorgabeValue = 100;
         else if (ThermeVorlaufTempVorgabe < 57) ThermeVorlaufTempVorgabeValue = 110;
         else if (ThermeVorlaufTempVorgabe < 62) ThermeVorlaufTempVorgabeValue = 120;
         else if (ThermeVorlaufTempVorgabe < 66) ThermeVorlaufTempVorgabeValue = 130;
         else if (ThermeVorlaufTempVorgabe < 69) ThermeVorlaufTempVorgabeValue = 140;
         else if (ThermeVorlaufTempVorgabe < 73) ThermeVorlaufTempVorgabeValue = 150;
         else if (ThermeVorlaufTempVorgabe < 76) ThermeVorlaufTempVorgabeValue = 160;
         else ThermeVorlaufTempVorgabeValue = 170;
         */
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
      ThermeControlState = ThermeControlStateSolarBetrieb;                                      // und in ThermeControlStateSolarBetrieb wechseln
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
        if (HKAktiv) {
          HKAktiv = false;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: HKAktiv = false"));
        }
        if (WWAktiv) {
          WWAktiv = false;
          if (thermeLogLevel > 0) Debug.println ( F("Therme: WWAktiv = false"));
        }
        HKAnforderungSeit = millis();
        WWAnforderungSeit = millis();
        if (thermeLogLevel > 1) {
          Debug.print  ( F("Therme: millis = "));
          Debug.println(millis());
          Debug.println( F("    ExternAnfordern = LOW"));
          Debug.println( F("    ThermeVorlaufTempVorgabeValue = 0"));
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



















