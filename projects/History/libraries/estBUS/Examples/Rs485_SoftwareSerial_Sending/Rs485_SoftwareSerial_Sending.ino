#include <SoftwareSerial.h>
#include <RS485_non_blocking.h>

SoftwareSerial rs485 (2, 3);  // receive pin, transmit pin
const byte ENABLE_PIN = 4;

size_t fWrite (const byte what)
{
  rs485.write (what);  
}

RS485 myChannel (NULL, NULL, fWrite, 0);

void setup ()
{
  rs485.begin (28800);
  pinMode (13        , OUTPUT);  // LED
  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite (13, LOW );
  myChannel.begin ();
}  // end of setup

const byte msg [] = "Hallo";

void loop ()
{ digitalWrite (13        , HIGH);
  digitalWrite (ENABLE_PIN, HIGH);
  myChannel.sendMsg (msg, sizeof (msg));
  digitalWrite (ENABLE_PIN, LOW );
  digitalWrite (13, LOW );
  delay (5000);   
}  // end of loop
