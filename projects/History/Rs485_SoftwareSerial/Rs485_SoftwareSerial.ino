//SoftwareSerial kann nicht gleichzeitig senden und empfangen!


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

size_t fWrite (const byte what)
{
  rs485.write (what);  
}

RS485 myChannel (fRead, fAvailable, fWrite, 20);

typedef union {
  byte bytes[7];
  struct {
    uint8_t fromDevice;
    uint16_t dataPoint;
    int32_t value;
  };
} 
msgType;

uint32_t lastMsgRecived = 0, w = 1000;

void setup ()
{
  Serial.begin  (57600);
  Serial.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  pinMode       (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite  (ENABLE_PIN, LOW );
  rs485.begin (28800);
  myChannel.begin ();
  randomSeed(analogRead(0));
}  // end of setup

void loop ()
{ 
  //  myChannel.sendMsg (msg, sizeof (msg));
  msgType msgIn, msgOut;
  if (myChannel.update ())
  { 
    Serial.print ("IN: ");
    Serial.write (myChannel.getData (), myChannel.getLength ()); 
    Serial.println ();
    for ( uint8_t i = 0; i < min( sizeof (msgIn), myChannel.getLength ()); i++) {
      msgIn.bytes[i] = myChannel.getData()[i];
    }
    digitalWrite (13        , HIGH);
    Serial.println ( msgOut.fromDevice );
    Serial.println ( msgOut.dataPoint  );
    Serial.println ( msgOut.value      );
    delay(100);
    digitalWrite (13        , LOW );
  }
  if ( millis() - lastMsgRecived > w) {
    Serial.println ("OUT: ");
    msgOut.fromDevice = 1;
    msgOut.dataPoint  = 2;
    msgOut.value      = 3;
  digitalWrite  (ENABLE_PIN, HIGH );
    myChannel.sendMsg (msgOut.bytes, sizeof (msgOut.bytes));
    lastMsgRecived = millis();
  digitalWrite  (ENABLE_PIN, LOW );
  }
}  // end of loop


