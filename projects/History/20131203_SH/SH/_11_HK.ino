#define HKPumpeAnPin         31

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
//Define Variables we'll be connecting to
double hKPidSoll, input, output;
//Specify the links and initial tuning parameters
PID myPID(&input, &output, &hKPidSoll, 2, 5, 1, DIRECT);
void myPIDChange(uint16_t Kp, uint16_t Ki, uint16_t Kd) {
  if ( Kp == ValueUnknown) Kp = myPID.GetKp() * 10;
  if ( Ki == ValueUnknown) Ki = myPID.GetKi() * 10;
  if ( Kd == ValueUnknown) Kd = myPID.GetKd() * 10;
  myPID.SetTunings(Kp / 10, Ki / 10, Kd / 10);
}

void HKInit() {
  pinMode     ( HKPumpeAnPin        , OUTPUT   );
  digitalWrite( HKPumpeAnPin        , LOW      );
  Serial2.begin( 9600);
  myPID.SetOutputLimits( 0, 1000);
  myPID.SetSampleTime(10000);
  myPID.SetMode( MANUAL);
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
    if ( ValueX10new1 < AnforderungFALSE_Zeitlimit) { // Die Einschaltkriterien sind nicht erfüllt
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

unsigned long  serialTime;

void HKVentil() {
  if ( myPID.GetMode() == MANUAL) {
    if ((    Values[_HKVorlaufTempSoll].ValueX10 != ValueUnknown
             &&  Values[_HKVorlaufTemp1].ValueX10   != ValueUnknown
             &&  Values[_HKVorlaufTemp2].ValueX10   != ValueUnknown
             &&  Values[_HKRuecklaufTemp2].ValueX10 != ValueUnknown)) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
      myPID.SetMode(AUTOMATIC);
      serialTime = millis();
    }
  }
  else {
    hKPidSoll = Values[_HKVorlaufTempSoll].ValueX10;
    input = Values[_HKVorlaufTemp2].ValueX10;
    myPID.Compute();
    ValueX10new1 = output;
    if (millis() > serialTime)
    {
      Debug.print  ( F ("V="));
      Debug.print  (     Values[_HKVorlaufTemp1].ValueX10    );
      Debug.print  ( F (" R="));
      Debug.print  (      Values[_HKRuecklaufTemp2].ValueX10 );
      Debug.print  ( F (" S="));
      Debug.print  (      Values[_HKVorlaufTempSoll].ValueX10);
      Debug.print  ( F (" output"));
      Debug.print  (      output                             );
      Debug.print  ( F (" ValueX10new1 "));
      Debug.println(      ValueX10new1   );
      serialTime += 1000;
    }
    if ( setValue( _HKVorlaufValue, ValueX10new1)) {
      if ( hkLogLevel > 1) {
        Debug.print   ( F("Therme: _HKVorlaufValue Zugewiesen: "));
        Debug.println ( Values[_HKVorlaufValue].ValueX10);
      }
    Serial2.print( ValueX10new1);
    Serial2.println( '#');
    }
  }
}









