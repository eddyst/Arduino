#define PumpeSolarServoEnabledPin 46
#define PumpeSolarServoPwmPin 48
#define PumpeSolarServoPwmMin 30
#define PumpeSolarServoPwmMax 175

#include <Servo.h> 
Servo servoSolar;  // create servo object to control a servo a maximum of eight servo objects can be created 

void   pumpenInit() {
  servoSolar.attach(PumpeSolarServoPwmPin);  // attaches the servo on pin 9 to the servo object 
  pinMode     (PumpeSolarServoEnabledPin, OUTPUT);  // Zum initialisieren Mischer erstmal zu drehen 
  digitalWrite(PumpeSolarServoEnabledPin, LOW   );

}

void pumpenDoEvents (){
  static uint8_t pos = 0;    // variable to store the servo position 
  static uint16_t lastMove = 0;
  if ((uint16_t)millis() - lastMove > 1000) {
    if (pos++ > PumpeSolarServoPwmMax) 
      pos = 0; 
    else
      pos++;
    servoSolar.write(pos);
    lastMove = millis();
  }
}

