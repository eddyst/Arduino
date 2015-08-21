#define HKPumpeAnPin         31


void HKInit(){
  pinMode     (HKPumpeAnPin        , OUTPUT   );
  digitalWrite(HKPumpeAnPin        , LOW      );

  Serial2.begin(9600);

}

void HKSolarbetriebBeginnt() {
  digitalWrite(HKPumpeAnPin, HIGH);
} 
void HKSolarbetriebEndet() {
  digitalWrite(HKPumpeAnPin, LOW );
} 

void   HKDoEvents() {
  HKAnforderung();
  HKVentil ();
}

void HKAnforderung() {
  ValueX10new1 = AnforderungNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if (Values[_HKAnforderung].ValueX10 < AnforderungTRUE) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_HKVorlaufTemp1].ValueX10      == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp1_UNKNOWN;
    else if ( Values[_HKVorlaufTemp1].ValueX10      >= Values[_HKVorlaufTempSoll].ValueX10) ValueX10new1 = AnforderungFALSE_Temp1;
    else if ( Values[_SpeicherA2    ].ValueX10      == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA2    ].ValueX10 - 10 >= Values[_HKVorlaufTempSoll].ValueX10) ValueX10new1 = AnforderungFALSE_Temp2; 
    else if ( Values[_SpeicherA3    ].ValueX10      == ValueUnknown                       ) ValueX10new1 = AnforderungFALSE_Temp3_UNKNOWN;
    else if ( millis() - AnforderungSeit > 300000                                         ) ValueX10new1 = AnforderungFALSE_Zeitlimit; 
    else                                                                                    ValueX10new1 = AnforderungTRUE;
    if ( ValueX10new1 < AnforderungFALSE_Zeitlimit){ // Die Einschaltkriterien sind nicht erfüllt
      AnforderungSeit = millis();      //Wartezeit  zurücksetzen
    }
  } 
  else {                                           //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10 + HKHysterese) ValueX10new1 = AnforderungTRUE_Temp1;
    else if ( Values[_SpeicherA2].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10 + HKHysterese) ValueX10new1 = AnforderungTRUE_Temp2;
    else if ( Values[_SpeicherA3].ValueX10 < Values[_HKVorlaufTempSoll].ValueX10              ) ValueX10new1 = AnforderungTRUE_Temp3;
    else                                                                              ValueX10new1 = AnforderungFALSE_Ausschaltkriterien;
  }
  if ( setValue( _HKAnforderung, ValueX10new1) && hkLogLevel > 1) {
    Debug.print   ( F("Therme: HKAnforderung Zugewiesen: "));
    Debug.println ( Values[_HKAnforderung].ValueX10);
  }
}

#define posIntervall 10000  // 10Sek
uint32_t HKVentilRechnenMillis;
void HKVentil() {
  ValueX10new1 = -1;
  if ( Values[_HKVorlaufTemp2].ValueX10 > 600 && Values[_HKVorlaufValue].ValueX10 != 0) {  // Zudrehen!!!!
    if (hkLogLevel > 0) Debug.println ("HK: ! >600 = soKALTwiesGEHT");
        ValueX10new1 = 0;
  } else {
    if (millis() - HKVentilRechnenMillis >= posIntervall ) {
      if ( Values[_HKVorlaufTempSoll].ValueX10 == ValueUnknown
       ||  Values[_HKVorlaufTemp1].ValueX10   == ValueUnknown
       ||  Values[_HKVorlaufTemp2].ValueX10   == ValueUnknown
       ||  Values[_HKRuecklaufTemp2].ValueX10 == ValueUnknown) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
      HKVentilRechnenMillis = millis() - posIntervall + 1000;
      } else {
        if (millis() - HKVentilRechnenMillis >= posIntervall ) {
          HKVentilRechnenMillis = millis();
          if ( Values[_HKVorlaufTemp1].ValueX10 == Values[_HKVorlaufTempSoll].ValueX10) {   // Bei Rücklauf = Soll drehen wir einfach auf Rücklauf sonst gibts #DIV/0
            if (hkLogLevel > 0) Debug.println ("HK: Sonderfall 1");
            ValueX10new1 =  0;
          } else if ( Values[_HKVorlaufTemp1].ValueX10 == Values[_HKRuecklaufTemp2].ValueX10) {  // Bei Vorlauf = Rücklauf gibts auch #DIV/0. Wir machen vorsichtshalber erstmal garnischt
            if (hkLogLevel > 0) Debug.println ("HK: Sonderfall 2");
            ValueX10new1 = Values[_HKVorlaufValue].ValueX10;
          } else {
    #define Fak       100 //Vermeidet Rundungsdifferenzen beim dividieren
    #define Schritte 1000
            Debug.print( F ("V=" )); Debug.print(Values[_HKVorlaufTemp1].ValueX10   );
            Debug.print( F (" R=")); Debug.print(Values[_HKRuecklaufTemp2].ValueX10 );
            Debug.print( F (" S=")); Debug.print(Values[_HKVorlaufTempSoll].ValueX10);
            tmpInt32_1 = Fak * (Values[_HKRuecklaufTemp2].ValueX10 - Values[_HKVorlaufTempSoll].ValueX10) / ( Values[_HKVorlaufTempSoll].ValueX10 - Values[_HKVorlaufTemp1].ValueX10);
            Debug.print( F (" tmpInt32_1.1=")); Debug.print( tmpInt32_1);
            tmpInt32_1 = (int32_t)Schritte * tmpInt32_1 / (Fak + tmpInt32_1);
            Debug.print( F (" tmpInt32_1.2=")); Debug.print(tmpInt32_1);
            static uint16_t vDelta = 0;
    #define vDeltaPlusMinusX10 100
            vDelta = vDelta + ((Values[_HKVorlaufTempSoll].ValueX10 - Values[_HKVorlaufTemp2].ValueX10) / 5);
            if ( vDelta > vDeltaPlusMinusX10) 
              vDelta = vDeltaPlusMinusX10;
            else if ( vDelta < -vDeltaPlusMinusX10)
              vDelta = -vDeltaPlusMinusX10;
            Debug.print(F(" vDelta=")); Debug.print(vDelta);
            tmpInt32_1 = tmpInt32_1 + vDelta;
            ValueX10new1 = min( Schritte, max( 0, tmpInt32_1));
            Debug.print(F(" ValueX10new1=")); Debug.println (ValueX10new1 );
          }
        }
      }
    }
  }
  if (ValueX10new1 >=0 ) {
    if ( setValue( _HKVorlaufValue, ValueX10new1)) {
      if ( hkLogLevel > 1) {
        Debug.print   ( F("Therme: _HKVorlaufValue Zugewiesen: "));
        Debug.println ( Values[_HKVorlaufValue].ValueX10);
      }
    }
    Serial2.print( ValueX10new1);
    Serial2.println( '#');
  }
}









