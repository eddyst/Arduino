#include <RS485_non_blocking.h>

size_t fWrite (const byte what)
{
  return Serial.write (what);  
}

RS485 myChannel (NULL, NULL, fWrite, 0);

void setup ()
{
  Serial.begin (115200);
  myChannel.begin ();
}  // end of setup

const byte msg [] = "Hello world";

void loop ()
{
  myChannel.sendMsg (msg, sizeof (msg));
  delay (1000);   
}  // end of loop
