#define HKPumpeAnPin         31

#define HKPinKalibrieren  45
#define HKPinSleep        39
#define HKPinDirection    41
#define HKPinDoStep       43

#define StepperSpeed  120
#define WAERMER HIGH
#define KAELTER !WAERMER

uint32_t pos = 0;
uint32_t Schritte         = 1000000;
int32_t SFehlerMin       = 0;
int32_t SFehlerMax       = 0;
uint8_t  Error            = 0;
#define ErrKeinSensor 1

void HKInit(){
  pinMode     (HKPumpeAnPin        , OUTPUT   );
  digitalWrite(HKPumpeAnPin        , LOW      );
   pinMode(HKPinKalibrieren, INPUT_PULLUP);
   pinMode(HKPinSleep      , OUTPUT);
   pinMode(HKPinDirection  , OUTPUT);
   pinMode(HKPinDoStep     , OUTPUT);
//   kalibrieren ();
  
}

void HKSolarbetriebBeginnt() {
  digitalWrite(HKPumpeAnPin, HIGH);
} 
void HKSolarbetriebEndet() {
  digitalWrite(HKPumpeAnPin, LOW );
} 

void   HKDoEvents() {
  HKAnforderung();
  //  HKVentil ();
}

void HKAnforderung() {
  ValueX10new1 = AnforderungNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if (Values[_HKAnforderung].ValueX10 < AnforderungTRUE) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_HKVorlaufTemp1].ValueX10 == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp1_UNKNOWN;
    else if ( Values[_HKVorlaufTemp1].ValueX10 >= Values[_HKVorlaufTempSoll].ValueX10) ValueX10new1 = AnforderungFALSE_Temp1;
    else if ( Values[_SpeicherA2    ].ValueX10 == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA2    ].ValueX10 >= Values[_HKVorlaufTempSoll].ValueX10) ValueX10new1 = AnforderungFALSE_Temp2; 
    else if ( Values[_SpeicherA3    ].ValueX10 == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp3_UNKNOWN;
    else if ( millis() - AnforderungSeit > 300000                                    ) ValueX10new1 = AnforderungFALSE_Zeitlimit; 
    else                                                                               ValueX10new1 = AnforderungTRUE;
    if ( ValueX10new1 < AnforderungFALSE_Zeitlimit){ // Die Einschaltkriterien sind nicht erfüllt
      AnforderungSeit = millis();      //Wartezeit  zurücksetzen
    }
  } 
  else {                                           //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10 + HKHysterese) ValueX10new1 = AnforderungTRUE_Temp2;
    else if ( Values[_SpeicherA2].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10 + HKHysterese) ValueX10new1 = AnforderungTRUE_Temp3;
    else                                                                              ValueX10new1 = AnforderungFALSE_Ausschaltkriterien;
  }
  if ( setValue( _HKAnforderung, ValueX10new1) && hkLogLevel > 1) {
    Debug.print   ( F("Therme: HKAnforderung Zugewiesen: "));
    Debug.println ( Values[_HKAnforderung].ValueX10);
  }
}
/*
void HKVentil() {
  if ( Values[_HKVorlaufTempSoll].ValueX10 != ValueUnknown
    ||  Values[_HKVorlaufTemp1].ValueX10 != ValueUnknown
    ||  Values[_HKVorlaufTemp2].ValueX10 != ValueUnknown
    ||  Values[_HKRuecklaufTemp2].ValueX10 != ValueUnknown) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
#define posIntervall 10000  // 10Sek
    static uint32_t rechnenMillis = millis() - posIntervall;
    if (millis() - rechnenMillis >= posIntervall ) {
      rechnenMillis = millis();
      ValueX10new1 = Values[_HKVorlaufValue].ValueX10;
      if ( Values[_HKVorlaufTemp2].ValueX10 > 600) {  // Zudrehen!!!!
        if (hkLogLevel > 0) Debug.println ("HK: ! >600 = soKALTwiesGEHT");
        ValueX10new1 = 0;
      }     
      else if (Values[_HKVorlaufTemp1].ValueX10<Values[_HKVorlaufTempSoll].ValueX10 && Values[_HKRuecklaufTemp2].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10) {
        ValueX10new1 = Schritte;
      }
      else if (Values[_HKVorlaufTemp1].ValueX10 > Values[_HKVorlaufTempSoll].ValueX10 && Values[_HKRuecklaufTemp2].ValueX10 > Values[_HKVorlaufTempSoll].ValueX10) {
        ValueX10new1 =  0;
      } 
      else {
#define Fak 1000 //Vermeidet Rundungsdifferenzen beim dividieren
        uint32_t bPos, iPos, sPos;
        if (Values[_HKVorlaufTempSoll].ValueX10 == Values[_HKRuecklaufTemp2].ValueX10) { //Division durch null abfangen
          bPos = 0;
        } 
        else {
          bPos = Schritte * Fak / ( Fak + Fak * ( Values[_HKVorlaufTemp1].ValueX10 - Values[_HKVorlaufTempSoll].ValueX10) / ( Values[_HKVorlaufTempSoll].ValueX10 - Values[_HKRuecklaufTemp2].ValueX10));
        }
        if (Values[_HKVorlaufTemp2].ValueX10 == Values[_HKRuecklaufTemp2].ValueX10) { //Division durch null abfangen
          iPos = 0;
        } 
        else {
          iPos = Schritte * Fak / ( Fak + Fak * ( Values[_HKVorlaufTemp1].ValueX10 - Values[_HKVorlaufTemp2].ValueX10      ) / ( Values[_HKVorlaufTemp2].ValueX10       - Values[_HKRuecklaufTemp2].ValueX10));
        }
        sPos = sqrt(pos * abs(pos + 0.001 * (bPos - iPos)));
        ValueX10new1 = sPos;
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
  }
}
*/






