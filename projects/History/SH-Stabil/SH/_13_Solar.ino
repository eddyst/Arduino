#define PumpeSolarAnPin              
#define PumpeSolarServoEnabledPin 46
#define PumpeSolarServoPwmPin 48
#define PumpeSolarServoPwmMin 30
#define PumpeSolarServoPwmMax 175

//#include <Servo.h> //
//Servo servoSolar;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   SolarInit() {
//  servoSolar.attach(PumpeSolarServoPwmPin);  // attaches the servo on pin 9 to the servo object 
//ToDo: Min und Max in Pulsewidht können hier übergeben werden!
  pinMode     (PumpeSolarServoEnabledPin, OUTPUT);   
  digitalWrite(PumpeSolarServoEnabledPin, LOW   );
}

void SolarDoEvents (){
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
}


