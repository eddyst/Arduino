#define WWPumpeAnPin           36 
#define WWVentilRuecklauf      37
#define WWPumpeServoPwmPin     38 
#define WWPumpeServoEnabledPin 47
#define WWZAnforderungPin      A1  
#define WWZirkulationPin       35

#include <Bounce.h>
Bounce wWZirkulationBouncer = Bounce(WWZAnforderungPin, 500, true); 

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

  pinMode     (WWZAnforderungPin , INPUT );
  pinMode     (WWZirkulationPin  , OUTPUT);
  digitalWrite(WWZirkulationPin  , LOW   );
  Values[_WWZirkulation].ValueX10 = 0;
}

uint32_t zirkuTimer;
void WWDoEvents (){
  WWAnforderungBerechnen ();
  WWPumpe ();

  ValueX10new1 = Values[_WWZirkulation].ValueX10;
  if ( ValueX10new1 == 10) {
    if ( millis() - zirkuTimer > 120000) {
      digitalWrite( WWZirkulationPin, LOW);
      ValueX10new1 = 0;
    }
  }
  else {
    if ( wWZirkulationBouncer.update()) {
      if ( wWZirkulationBouncer.fallingEdge()) {
       digitalWrite( WWZirkulationPin, HIGH);
       ValueX10new1 = 10;
       zirkuTimer = millis();
      }  
    }
  }
  if ( setValue( _WWZirkulation, ValueX10new1) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWZirkulation Zugewiesen: "));
    Debug.println   ( Values[_WWZirkulation].ValueX10);
  }


  if ( digitalRead( WWVentilRuecklauf) == LOW) {
    if ( Values[ _WWRuecklaufTemp].ValueX10 != ValueUnknown
      && Values[ _SpeicherA2     ].ValueX10 != ValueUnknown
      && Values[ _SpeicherA3     ].ValueX10 != ValueUnknown
      && (  Values[ _WWRuecklaufTemp].ValueX10 >= Values[ _SpeicherA2].ValueX10 
      ||Values[ _WWRuecklaufTemp].ValueX10 >= Values[ _SpeicherA3].ValueX10 + 20)
      )  {
      digitalWrite( WWVentilRuecklauf, HIGH);
    }
  } 
  else {
    if ( Values[ _WWRuecklaufTemp].ValueX10 < Values[ _SpeicherA3].ValueX10) {
      digitalWrite( WWVentilRuecklauf, LOW); 
    } 
  }
  if ( setValue( _WWVentil, (digitalRead( WWVentilRuecklauf) == HIGH) ? 10: 0) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWVentil Zugewiesen: "));
    Debug.println   ( Values[_WWVentil].ValueX10);
  }
}

void WWAnforderungBerechnen () {
  ValueX10new1 = Values[_WWAnforderung].ValueX10;
  static uint32_t AnforderungSeit;
  if ( ValueX10new1 <= AnforderungFALSE_Zeitlimit) { // Es ist noch keine Anforderung aktiv -> prüfen was gegen einschalten spricht
    if      ( Values[_WWSpeicherTemp1].ValueX10 == ValueUnknown                                      ) ValueX10new1 = AnforderungFALSE_Temp1_UNKNOWN;
    else if ( Values[_WWSpeicherTemp1].ValueX10 >= Values[_WWSpeicherTempSoll].ValueX10)               ValueX10new1 = AnforderungFALSE_Temp1;
    else if ( Values[_SpeicherA1     ].ValueX10 == ValueUnknown                                      ) ValueX10new1 = AnforderungFALSE_Temp2_UNKNOWN;
    else if ( Values[_SpeicherA1     ].ValueX10 >= Values[_WWSpeicherTempSoll].ValueX10 + WWSpreizung) ValueX10new1 = AnforderungFALSE_Temp2; 
    else if ( Values[_SpeicherA2     ].ValueX10 == ValueUnknown                                      ) ValueX10new1 = AnforderungFALSE_Temp3_UNKNOWN; //Weil als ausschaltkriterium
    else if (ValueX10new1 == AnforderungFALSE_Zeitlimit) {
      if (millis() - AnforderungSeit > 30000 ) {
        ValueX10new1 = AnforderungTRUE;
      }
    } 
    else {
      AnforderungSeit = millis();      //Wartezeit  Starten
      ValueX10new1 = AnforderungFALSE_Zeitlimit;
    } 
  } 
  else if (ValueX10new1 == AnforderungTRUE) {  //Die Anforderung ist bereits aktiv -> kömmer nu endlich AUS?
    if      ( Values[_WWSpeicherTemp1].ValueX10 > Values[_WWSpeicherTempSoll].ValueX10 + WWHysterese                   ) ValueX10new1 = AnforderungFALSE_AusTemp1;
    //    else if ( Values[_SpeicherA1     ].ValueX10 > Values[_WWSpeicherTempSoll].ValueX10 + WWHysterese + WWSpreizung * 2 ) ValueX10new1 = AnforderungFALSE_AusTemp2;
    else if ( Values[_SpeicherA2     ].ValueX10 > Values[_WWSpeicherTempSoll].ValueX10 + WWHysterese + WWSpreizung     ) ValueX10new1 = AnforderungFALSE_AusTemp3;
    else {
      ValueX10new1 = AnforderungTRUE;
      AnforderungSeit = millis();      //Wartezeit  Starten
    }
  } 
  else {
    if (millis() - AnforderungSeit > 30000 ) 
      ValueX10new1 = AnforderungNOT_INITIALIZED;
  }
  if ( setValue( _WWAnforderung, ValueX10new1) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWAnforderung Zugewiesen: "));
    Debug.println ( Values[_WWAnforderung].ValueX10    );
  }
}
#define WWPumpe_AusWWSpeicherTemp1GroesserSpeicherA1und2Minus2C -60
#define WWPumpe_AusWWSpeicherTemp1GroesserWWtMaxMinus2C         -50
#define WWPumpe_KeinSolarBetrieb                                -40
#define WWPumpe_Aus                                             -35
#define WWPumpe_WWSpeicherTemp1Unknown                          -30
#define WWPumpe_WWSpeicherTemp1GroesserWWtMaxMinus2C            -20
#define WWPumpe_WWSpeicherTemp1GroesserSpeicherA1und2Minus2C    -10

void WWPumpe () {
  ValueX10new1 = Values[_WWPumpeProzent].ValueX10;
  static uint32_t wwPumpeSperre;
  if ( ValueX10new1 > WWPumpe_Aus
    || millis() - wwPumpeSperre > 60000) {
    if( digitalRead(WWPumpeAnPin) == LOW   ){  //Pumpe ist aus
      if ( Values[_SteuerungStatus].ValueX10 <= SteuerungStatusNotfallBetrieb)
        ValueX10new1 = WWPumpe_KeinSolarBetrieb;
      else if( Values[_WWSpeicherTemp1].ValueX10 >  WWtMax - 20) 
        ValueX10new1 = WWPumpe_WWSpeicherTemp1GroesserWWtMaxMinus2C;      //is schon 2 C vor Temp.begrenzer und lohnt ni mehr
      else if( Values[_WWSpeicherTemp1].ValueX10 >  max( Values[ _SpeicherA1].ValueX10, Values[ _SpeicherA2].ValueX10) - 20) 
        ValueX10new1 = WWPumpe_WWSpeicherTemp1GroesserSpeicherA1und2Minus2C;      
      else if( Values[_WWSpeicherTemp1].ValueX10 == ValueUnknown) 
        ValueX10new1 = WWPumpe_WWSpeicherTemp1Unknown;                    //Sonst gibts nix zu regeln
      else {
        ValueX10new1 = 1000;
        digitalWrite(WWPumpeAnPin, HIGH);
      }
    } 
    else { //Pumpe ist an
      if ( Values[_SteuerungStatus].ValueX10 <= SteuerungStatusNotfallBetrieb)
        ValueX10new1 = WWPumpe_KeinSolarBetrieb;
      else if ( Values[ _WWSpeicherTemp1].ValueX10 > WWtMax)       //Ausschaltkriterium im Sommer wenn genug Solar vorhanden
        ValueX10new1 = WWPumpe_AusWWSpeicherTemp1GroesserWWtMaxMinus2C;   
      else if( Values[ _WWSpeicherTemp1].ValueX10 > max( Values[ _SpeicherA1].ValueX10, Values[ _SpeicherA2].ValueX10) - 10) //Ausschaltkriterium ohne Solareintrag - weiter pumpen würde das WW abkühlen
        ValueX10new1 = WWPumpe_AusWWSpeicherTemp1GroesserSpeicherA1und2Minus2C;   
      if ( ValueX10new1 < WWPumpe_Aus) {
        digitalWrite(WWPumpeAnPin, LOW);
        wwPumpeSperre = millis();
      } 
    }
  }
  if ( setValue( _WWPumpeProzent, ValueX10new1) && wwLogLevel > 1) {
    Debug.print   ( F("WW: WWPumpeProzent Zugewiesen: "));
    Debug.println   ( Values[_WWPumpeProzent].ValueX10);
  }
}












