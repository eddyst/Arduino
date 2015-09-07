#include <SoftwareSerial.h>
#include <RS485_non_blocking.h>

SoftwareSerial rs485 (2, 3);  // receive pin, transmit pin
const byte ENABLE_PIN = 4;

int fAvailable ()
{
  return rs485.available ();  
}

int fRead ()
{ 
  return rs485.read (); 
}


RS485 myChannel (fRead, fAvailable, NULL, 20);

void setup ()
{
  Serial.begin  (115200);
  Serial.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  pinMode       (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite  (ENABLE_PIN, LOW );

  rs485.begin (28800);
  myChannel.begin ();
}  // end of setup

void loop ()
{ 
  if (myChannel.update ())
  {
    Serial.print ("IN: ");
    Serial.write (myChannel.getData (), myChannel.getLength ()); 
    Serial.println ();
    digitalWrite (13        , HIGH);
    delay(100);
    digitalWrite (13        , LOW );
  }
}  // end of loop

