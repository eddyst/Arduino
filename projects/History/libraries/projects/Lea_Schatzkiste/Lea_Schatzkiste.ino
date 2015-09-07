/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object 
  zu();
} 
 
void loop() 
{ 
  delay(1000);
  schlunzeln();
  delay(5000);
  auf();
  delay(10000);
  zu();
} 

void schlunzeln () {
  for(pos = 160; pos>=100; pos = pos - 1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(100);                       // waits 15ms for the servo to reach the position 
  } 
}

void auf () {
  for( ; pos>=20; pos = pos - 1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

void zu () {
  myservo.write(160);              // tell servo to go to position in variable 'pos' 
}
