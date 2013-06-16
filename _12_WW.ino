#define PumpeWWAnPin             32 
#define PumpeWWServoPwmPin       34
//#define PumpeWWServoEnabledPin 

//#define PumpeWWServoPwmMin 30
//#define PumpeWWServoPwmMax 175

//#include <Servo.h> //
//Servo servoWW;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   WWInit() {
  pinMode     (PumpeWWAnPin, OUTPUT);   
  digitalWrite(PumpeWWAnPin, LOW   );

  //  pinMode     (PumpeWWServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeWWServoEnabledPin, LOW   );

  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  servoWW.attach(PumpeWWServoPwmPin);  // attaches the servo on pin 9 to the servo object 
}

void WWDoEvents (){
  if ( digitalRead(PumpeWWAnPin) == LOW   ){
    if ( Values[_WWSpeicherTemp1].ValueX10 != ValueUnknown
      && Values[_WWSpeicherTemp1].ValueX10 < WWSollTempVorgabe * 10 
      && Values[_SpeicherA1     ].ValueX10 != ValueUnknown
      && Values[_SpeicherA1     ].ValueX10 > Values[_WWSpeicherTemp1].ValueX10) {           //Einschaltkriterium erfüllt
      digitalWrite(PumpeWWAnPin, HIGH   );
    }
  }
  else {
    if ( Values[_WWSpeicherTemp1].ValueX10 > WWSollTempVorgabe * 10 + WWHysterese
      || Values[_SpeicherA1     ].ValueX10 <= Values[_WWSpeicherTemp1].ValueX10) {           //Einschaltkriterium erfüllt
      digitalWrite(PumpeWWAnPin, LOW   );
    }
  }
  /*
  static uint8_t pos = 0;    // variable to store the servo position 
   static uint32_t lastMove = 0;
   if (millis() - lastMove > 1000) {
   if (pos++ > PumpeWWServoPwmMax) 
   pos = 0; 
   else
   pos++;
   if (pumpLogLevel > 0) {
   Debug.print(F("pump: Pos = "));
   Debug.println(pos);
   }
   //    servoWW.write(pos);
   lastMove = millis();
   }
   */
}


