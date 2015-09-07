#define PumpeSolarAnPin             38              
//#define PumpeSolarServoEnabledPin 46
//#define PumpeSolarServoPwmPin 48
//#define PumpeSolarServoPwmMin 30
//#define PumpeSolarServoPwmMax 175

//#include <Servo.h> //
//Servo servoSolar;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   SolarInit() {
  pinMode     (PumpeSolarAnPin    , OUTPUT);   
  digitalWrite(PumpeSolarAnPin    , LOW   );
  //  servoSolar.attach(PumpeSolarServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  pinMode     (PumpeSolarServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeSolarServoEnabledPin, LOW   );
}

#define SolarStatus_INIT                         0
#define SolarStatus_UNKNOWN_KollektorWTRuecklauf 1
#define SolarStatus_UNKNOWN_SpeicherA5           2
#define SolarStatus_AUS                          3
#define SolarStatus_AN                           4

void SolarDoEvents (){
  uint32_t zp = g_Clock.GetTimestamp();
  static uint32_t millis1 = millis();
  static uint8_t Status = 0;
  switch (Status){
  case SolarStatus_INIT:
    Status = SolarStatus_UNKNOWN_KollektorWTRuecklauf;
    break;
  case SolarStatus_UNKNOWN_KollektorWTRuecklauf:
    if (Values[_KollektorWTRuecklauf].ValueX10 != ValueUnknown) 
      Status = SolarStatus_UNKNOWN_SpeicherA5;
    break;
  case SolarStatus_UNKNOWN_SpeicherA5:
    if (Values[_SpeicherA5].ValueX10 != ValueUnknown) 
      Status = SolarStatus_AUS;
    break;
  case SolarStatus_AUS: //Anschalten?
    if (Values[_KollektorWTRuecklauf].ValueX10 > Values[_SpeicherA5].ValueX10 + 5) {
      digitalWrite(PumpeSolarAnPin, HIGH);
      Status = SolarStatus_AN;
    }
    break;
  case SolarStatus_AN:
    if (Values[_KollektorWTRuecklauf].ValueX10 < Values[_SpeicherA5].ValueX10){// Uppps Wärme ist weg
      digitalWrite(PumpeSolarAnPin, LOW);
      Status = SolarStatus_AUS;
    }  else {
/*
  static uint8_t pos = 0;    // variable to store the servo position 
   static uint32_t lastMove = 0;
   if (millis() - lastMove > 1000) {
   if (pos++ > PumpeSolarServoPwmMax) 
   pos = 0; 
   else
   pos++;
   if (solarLogLevel > 0) {
   Debug.print(F("pump: Pos = "));
   Debug.println(pos);
   }
   //    servoSolar.write(pos);
   lastMove = millis();
   }
 */      
    }
    break;
  }
}









