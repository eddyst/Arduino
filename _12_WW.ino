#define WWPumpeAnPin           36 
#define WWVentilRuecklauf      37
#define WWPumpeServoPwmPin     38 
#define WWPumpeServoEnabledPin 47

//#define PumpeWWServoPwmMin 30
//#define PumpeWWServoPwmMax 175

//#include <Servo.h> //
//Servo servoWW;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   WWInit() {
  pinMode     (WWPumpeAnPin, OUTPUT);   
  digitalWrite(WWPumpeAnPin, LOW   );

  //  pinMode     (PumpeWWServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeWWServoEnabledPin, LOW   );

  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  servoWW.attach(PumpeWWServoPwmPin);  // attaches the servo on pin 9 to the servo object 

  pinMode     (WWVentilRuecklauf, OUTPUT);   
  digitalWrite(WWVentilRuecklauf, LOW   );

}

void WWDoEvents (){
  int8_t  Anforderung = AnforderungNOT_INITIALIZED;
  static uint32_t AnforderungSeit;
  if ( WWSollTempVorgabe > 60 - WWHysterese)
    WWSollTempVorgabe = 60 - WWHysterese;
  if (Values[_WWAnforderung].ValueX10 < AnforderungTRUE_ZeitlimitStart) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_WWSpeicherTemp1].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp1_UNKNOWN;
    else if ( Values[_WWSpeicherTemp1].ValueX10 >= WWSollTempVorgabe * 10) Anforderung = AnforderungFALSE_Temp1;
    else if ( Values[_SpeicherA1     ].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA1     ].ValueX10 >= WWSollTempVorgabe * 10) Anforderung = AnforderungFALSE_Temp2; 
    else if ( Values[_SpeicherA2     ].ValueX10 == ValueUnknown)           Anforderung = AnforderungFALSE_Temp3_UNKNOWN;
    else                                                                   Anforderung = AnforderungTRUE_Zeitlimit;
  } 
  else if (Values[_WWAnforderung].ValueX10 == AnforderungTRUE_ZeitlimitStart) {  
    AnforderungSeit = millis();      //Wartezeit  Starten
    Anforderung = AnforderungTRUE_Zeitlimit;
  } 
  else if (Values[_WWAnforderung].ValueX10 == AnforderungTRUE_Zeitlimit) {
    if (millis() - AnforderungSeit > 30000 ) {
      Anforderung = AnforderungTRUE;
    }
  } 
  else if (Values[_WWAnforderung].ValueX10 == AnforderungTRUE) {  //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_SpeicherA1     ].ValueX10 > ( WWSollTempVorgabe + WWHysterese) * 10) Anforderung = AnforderungTRUE_Temp2;
    else if ( Values[_SpeicherA2     ].ValueX10 > ( WWSollTempVorgabe + WWHysterese) * 10) Anforderung = AnforderungTRUE_Temp3;
    else                                                                                   Anforderung = AnforderungFALSE_Ausschaltkriterien;
  } 
  else Anforderung = AnforderungNOT_INITIALIZED;

  if ( setValue( _WWAnforderung, Anforderung) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWAnforderung Zugewiesen: "));
    Debug.println   ( Values[_WWAnforderung].ValueX10);
  }

  uint16_t sMax = Values[ _SpeicherA1].ValueX10;
  if (sMax < Values[ _SpeicherA1].ValueX10) sMax = Values[ _SpeicherA2].ValueX10;

  uint8_t pos = Values[_WWPumpeProzent].ValueX10;    // variable to store the servo position 
  if ( digitalRead(WWPumpeAnPin) == LOW   ){
    if ( Solarbetrieb
      && Values[_WWSpeicherTemp1].ValueX10 != ValueUnknown  //Sonst gibts nix zu regeln
      && Values[_WWSpeicherTemp1].ValueX10 < 550            //is schon nahe am Temp.begrenzer und lohnt ni mehr
      && Values[_WWSpeicherTemp1].ValueX10 < sMax - 20) {   //Einschalten bringt noch was
      digitalWrite(WWPumpeAnPin, HIGH   );
      pos = 100;
    }
  }
  else {
    if ( !Solarbetrieb
      || Values[ _WWSpeicherTemp1].ValueX10 > 570         //Ausschaltkriterium im Sommer wenn genug Solar vorhanden
      || sMax < Values[ _WWSpeicherTemp1].ValueX10 + 10) {//Ausschaltkriterium ohne Solareintrag - weiter pumpen würde das WW abkühlen
      digitalWrite(WWPumpeAnPin, LOW   );
      pos = 0;
    } 
  }
  if ( setValue( _WWPumpeProzent, pos) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWPumpeProzent Zugewiesen: "));
    Debug.println   ( Values[_WWPumpeProzent].ValueX10);
  }

  if ( digitalRead( WWVentilRuecklauf) == LOW) {
    if ( Values[ _WWRuecklaufTemp].ValueX10 != ValueUnknown
      && Values[ _SpeicherA2     ].ValueX10 != ValueUnknown
      && Values[ _WWRuecklaufTemp].ValueX10 > Values[ _SpeicherA2].ValueX10 + 5) {
      digitalWrite( WWVentilRuecklauf, HIGH);
    }
  } 
  else {
    if ( Values[ _WWRuecklaufTemp].ValueX10 < Values[ _SpeicherA2].ValueX10) {
      digitalWrite( WWVentilRuecklauf, LOW); 
    } 
  }
  if ( setValue( _WWVentil, iif(digitalRead( WWVentilRuecklauf) == HIGH, 10, 0)) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWVentil Zugewiesen: "));
    Debug.println   ( Values[_WWVentil].ValueX10);
  }
}








