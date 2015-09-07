/*
 *  estTimer2 library example
 *  Sept 2013
 */
     
#include "estTimer2.h"

#define ledPin 13
void setup()
{
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, LOW);
  estTimer2::set(100, 1.0/10000, callback); // call every 500 1ms "ticks"
//  estTimer2::set(500, callback); // MsTimer2 style is also supported
  estTimer2::start();
}
     
void callback()
{
  static boolean output = HIGH;
  digitalWrite(ledPin, output);
  output = !output;
}
     
void loop()
{
  // your program here...
}
     

