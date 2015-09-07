// this sketch cycles three servos at different rates

#include <ServoTimer2.h>  // the servo library

// define the pins for the servos
#define rollPin  4
#define pitchPin 5


ServoTimer2 servoRoll;    // declare variables for up to eight servos
ServoTimer2 servoPitch;

void setup() {
  Serial.begin(57600);

  servoRoll.attach(rollPin);     // attach a pin to the servos and they will start pulsing
  servoRoll.write(2000);
  delay(10000);
  servoRoll.write(1000);
  delay(2000);
  servoRoll.write(1500);
  delay(2000);
  servoPitch.attach(pitchPin);
  
}


// this function just increments a value until it reaches a maximum
int incPulse(int val, int inc){
   if( val + inc  > 2000 )
	return 1000 ;
   else
	 return val + inc;
}

void loop()
{
 int val;

   val = incPulse( servoRoll.read(), 1);
   Serial.print   (val);
   Serial.print   (" - ");
   servoRoll.write(val);

   val =  incPulse( servoPitch.read(), 2);
   servoPitch.write(val);
   Serial.print    (val);
   Serial.println  ("");
 
   delay(100);
}
