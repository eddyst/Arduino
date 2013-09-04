#define SolarVentil1Pin           23
#define SolarVentil2Pin           25
#define SolarVentil3Pin           27
#define SolarPumpeAnPin           40              
#define SolarPumpeServoPwmPin     42
#define SolarPumpeServoEnabledPin 49

//#define PumpeSolarServoPwmMin 30
//#define PumpeSolarServoPwmMax 175

//#include <Servo.h> //
//Servo servoSolar;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   SolarInit() {
  pinMode     ( SolarVentil1Pin    , OUTPUT);
  digitalWrite( SolarVentil1Pin    , LOW);  
  pinMode     ( SolarVentil2Pin    , OUTPUT);   
  digitalWrite( SolarVentil2Pin    , LOW);  
  pinMode     ( SolarVentil3Pin    , OUTPUT);   
  digitalWrite( SolarVentil3Pin    , LOW);  
  pinMode     ( SolarPumpeAnPin    , OUTPUT);   
  digitalWrite( SolarPumpeAnPin    , LOW   );
  //  servoSolar.attach(PumpeSolarServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  //ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  //  pinMode     (PumpeSolarServoEnabledPin, OUTPUT);   
  //  digitalWrite(PumpeSolarServoEnabledPin, LOW   );
}

#define SolarStatus_INIT                        -3
#define SolarStatus_UNKNOWN_KollektorWTVorlauf  -2
#define SolarStatus_UNKNOWN_SpeicherA5          -1
#define SolarStatus_AUS                          0
#define SolarStatus_AN_2                        40
#define SolarStatus_AN_3                        30
#define SolarStatus_AN_4                        20
#define SolarStatus_AN_5                        10

void SolarDoEvents (){
  static int8_t Status = SolarStatus_INIT;
  switch (Status){
  case SolarStatus_AUS: // Leider der häufigste Status aber vielleicht kömmer Anschalten?
    if (Values[_KollektorWTVorlauf].ValueX10 > Values[_SpeicherA5].ValueX10 + 50) {
      digitalWrite(SolarPumpeAnPin, HIGH);
      Status = SolarStatus_AN_5;
    }
    break;
  case SolarStatus_AN_2: // Lieblingsmodus weil volle Wärme vom Dach
    if (Values[ _SpeicherA2].ValueX10 > Values[ _SolarVorlauf].ValueX10) { // Zu kalt - eine Stufe runter schalten 
      digitalWrite( SolarVentil1Pin, LOW); 
      Status = SolarStatus_AN_3;
    } // Höher Einspeisen geht nimmer
    break;
  case SolarStatus_AN_3:
    if (Values[ _SpeicherA3].ValueX10 > Values[ _SolarVorlauf].ValueX10) { // Zu kalt - eine Stufe runter schalten 
      digitalWrite( SolarVentil2Pin, LOW); 
      Status = SolarStatus_AN_4;
    } 
    else if (Values[ _SpeicherA2].ValueX10 != ValueUnknown 
      && Values[ _SpeicherA2].ValueX10 + 5 < Values[ _SolarVorlauf].ValueX10) { // Wärmer als nächste Ebene - eine Stufe hoch schalten
      digitalWrite( SolarVentil1Pin, HIGH); 
      Status = SolarStatus_AN_2;
    }
    break;
  case SolarStatus_AN_4:
    if (Values[ _SpeicherA4].ValueX10 > Values[ _SolarVorlauf].ValueX10) { // Zu kalt - eine Stufe runter schalten 
      digitalWrite( SolarVentil3Pin, LOW); 
      Status = SolarStatus_AN_5;
    } 
    else if (Values[ _SpeicherA3].ValueX10 != ValueUnknown 
      && Values[ _SpeicherA3].ValueX10 + 5 < Values[ _SolarVorlauf].ValueX10) { // Wärmer als nächste Ebene - eine Stufe hoch schalten
      digitalWrite( SolarVentil2Pin, HIGH); 
      Status = SolarStatus_AN_3;
    }
    break;
  case SolarStatus_AN_5: 
    if (Values[_KollektorWTVorlauf].ValueX10 < Values[_SpeicherA5].ValueX10 + 5){// Wenn keine Wärmeleistung mehr dann aus
     digitalWrite(SolarPumpeAnPin, LOW);
      Status = SolarStatus_AUS;
    } else { // ansonsten püfen ob warm genug für Ebene 4
      if ( Values[ _SpeicherA4].ValueX10   != ValueUnknown                     
        && Values[ _SolarVorlauf].ValueX10 != ValueUnknown
        && Values[ _SpeicherA4].ValueX10 + 5 < Values[ _SolarVorlauf].ValueX10) {
        digitalWrite( SolarVentil3Pin, HIGH); 
        Status = SolarStatus_AN_4; //Das unterste der 3 Ventile schaltet - Wasser kommt auf Ebene von Sensor 4 an
      } 
    }
    break;
  case SolarStatus_INIT:
    Status = SolarStatus_UNKNOWN_KollektorWTVorlauf;
    break;
  case SolarStatus_UNKNOWN_KollektorWTVorlauf:
    if (Values[_KollektorWTVorlauf].ValueX10 != ValueUnknown) 
      Status = SolarStatus_UNKNOWN_SpeicherA5;
    break;
  case SolarStatus_UNKNOWN_SpeicherA5:
    if (Values[_SpeicherA5].ValueX10 != ValueUnknown) 
      Status = SolarStatus_AUS;
    break;
  default: 
    Status = SolarStatus_INIT;
    break;
  }
  /* Wenn Pumpe an Geschwindigkeit regeln
   static uint8_t pos = 0;    // variable to store the servo position 
   static uint32_t lastMove = 0;
   if (millis() - lastMove > 1000) {
   if (pos++ > PumpeSolarServoPwmMax) 
   pos = 0; 
   else
   pos++;
   if (solarLogLevel > 0) {
   Debug.print  ( F( "pump: Pos = "));
   Debug.println(pos);
   }
   //    servoSolar.write(pos);
   lastMove = millis();
   }
   */
  if ( setValue( _SolarStatus , Status) && solarLogLevel > 0) {
    Debug.print  ( F( "Solar: Status Zugewiesen: "));           
    Debug.println(Status); 
  }
}









