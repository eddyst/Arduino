#define HKPumpeAnPin         31
#define HKMischerBewegenPin  32
#define HKMischerRichtungPin 33

#define KAELTER LOW
#define WAERMER HIGH
#define HKBEWEGEN HIGH
#define HKSTOP    LOW
void HKInit(){
  pinMode     (HKPumpeAnPin        , OUTPUT   );
  digitalWrite(HKPumpeAnPin        , LOW      );
  pinMode     (HKMischerRichtungPin, OUTPUT   );  // Zum initialisieren Mischer erstmal zu drehen 
  digitalWrite(HKMischerRichtungPin, KAELTER  );
  pinMode     (HKMischerBewegenPin , OUTPUT   );
  digitalWrite(HKMischerBewegenPin , HKSTOP);
  Values[_HKVorlaufValue].ValueX10 = 0;
}

void HKSolarbetriebBeginnt() {
  digitalWrite(HKPumpeAnPin, HIGH);
} 
void HKSolarbetriebEndet() {
  digitalWrite(HKPumpeAnPin, LOW );
} 

void   HKDoEvents() {
  //HKMischerRichtungPin  HKMischerBewegenPin  HKSollTempVorgabe
  int8_t  Anforderung = AnforderungNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if (Values[_HKAnforderung].ValueX10 < AnforderungTRUE) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_HKVorlaufTemp1].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp1_UNKNOWN;
    else if ( Values[_HKVorlaufTemp1].ValueX10 >= HKSollTempVorgabe * 10) Anforderung = AnforderungFALSE_Temp1;
    else if ( Values[_SpeicherA2    ].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA2    ].ValueX10 >= HKSollTempVorgabe * 10) Anforderung = AnforderungFALSE_Temp2; 
    else if ( Values[_SpeicherA3    ].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp3_UNKNOWN;
    else if ( millis() - AnforderungSeit > 300000 )                       Anforderung = AnforderungTRUE_Zeitlimit; 
    else                                                                  Anforderung = AnforderungTRUE;
    if ( Anforderung < AnforderungTRUE_Zeitlimit){ // Die Einschaltkriterien sind nicht erfüllt
      AnforderungSeit = millis();      //Wartezeit  zurücksetzen
    }
  } 
  else {                                           //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1].ValueX10 < ( HKSollTempVorgabe + HKHysterese) * 10) Anforderung = AnforderungTRUE_Temp2;
    else if ( Values[_SpeicherA2].ValueX10 < ( HKSollTempVorgabe + HKHysterese) * 10) Anforderung = AnforderungTRUE_Temp3;
    else                                                                              Anforderung = AnforderungFALSE_Ausschaltkriterien;
  }
  if ( setValue( _HKAnforderung, Anforderung) && hkLogLevel > 1) {
    Debug.print   ( F("Therme: HKAnforderung Zugewiesen: "));
    Debug.println ( Values[_HKAnforderung].ValueX10);
  }

  //  if (Solarbetrieb) {
  if ( Values[_HKVorlaufTemp2].ValueX10 != ValueUnknown) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
    if ( Values[_HKVorlaufTemp2].ValueX10 > 600) {  // Zudrehen!!!!
      if (hkLogLevel > 0) Debug.println ("HK: ! >600 = soKALTwiesGEHT");
      digitalWrite (HKMischerRichtungPin, KAELTER );
      digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
    }     
    else {
      static uint32_t lastMove = millis() - 60000;
      static int16_t lTemp = Values[_HKVorlaufTemp2].ValueX10;
      if (millis() - lastMove > 60000) {                         // Wenn min ? Sekunden seit letzter Bewegung
        //Wir haben keine genaue Info zur Ventilpossition deswegen zählen wir hilfsweise bei jeder Bewegung die Schritte in eine Richtung
        if ( lTemp - Values[_HKVorlaufTemp2].ValueX10 < 0) { //Temperatur ist gestiegen
          if ( hkLogLevel > 1) {
            Debug.print  (F("HK: Temp gestiegen"));
            Debug.print  (lTemp);
            Debug.print  (F(" -> "));
            Debug.println(Values[_HKVorlaufTemp2].ValueX10);
          }
          int16_t d = -1 * abs(Values[_HKVorlaufTemp2].ValueX10 - HKSollTempVorgabe * 10) * 120 + 300;
          hkMove (d);
        } 
        else if ( lTemp - Values[_HKVorlaufTemp2].ValueX10 > 0 ) { 
          if ( hkLogLevel > 1) {
            Debug.print  (F("HK: Temp gefallen"));
            Debug.print  (lTemp);
            Debug.print  (F(" -> "));
            Debug.println(Values[_HKVorlaufTemp2].ValueX10);
          }
            int16_t d = abs(Values[_HKVorlaufTemp2].ValueX10 - HKSollTempVorgabe * 10) * 120 + 400;
            hkMove (d);
        }
        lTemp = Values[_HKVorlaufTemp2].ValueX10;
        lastMove=millis();  
      }
    }
  }
  // } 
}

void hkMove ( int16_t d) {
  if ( hkLogLevel > 1) {
    Debug.println ( F( " move  "));
    Debug.println (d);
  }
  if ( d < -1000) d = -1000;
  if ( d >  1000) d =  1000;
  if ( d < 0 )
    digitalWrite (HKMischerRichtungPin, KAELTER );
  else
    digitalWrite (HKMischerRichtungPin, WAERMER );
  digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
  delay(abs(d));
  digitalWrite (HKMischerBewegenPin , HKSTOP);
  if ( (Values[_HKVorlaufValue].ValueX10 > 0 && d < 0) 
    || (Values[_HKVorlaufValue].ValueX10 < 0 && d > 0) )
    Values[_HKVorlaufValue].ValueX10 = 0;
  Values[_HKVorlaufValue].ValueX10 += d / 100;
  Values[_HKVorlaufValue].Changed = 1;
  if (hkLogLevel > 0) {
    Debug.print (F("    HKVorlaufValue = "));
    Debug.println(Values[_HKVorlaufValue].ValueX10);
  }
}











