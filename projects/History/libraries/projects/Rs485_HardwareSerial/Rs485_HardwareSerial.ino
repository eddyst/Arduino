#include <SoftwareSerial.h>
#include <RS485_non_blocking.h>

SoftwareSerial Debug (2, 3);  // receive pin, transmit pin
const byte ENABLE_PIN = 4;

int fAvailable ()
{
  return Serial.available ();  
}

int fRead ()
{ 
  return Serial.read (); 
}

size_t fWrite (const byte what)
{
  Serial.write (what);  
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
  Debug.begin  (57600);
  Debug.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  pinMode       (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite  (ENABLE_PIN, LOW );
  Serial.begin (9600);
  myChannel.begin ();
  randomSeed(analogRead(0));
}  // end of setup

void loop ()
{ 
  //  myChannel.sendMsg (msg, sizeof (msg));
  msgType msgIn, msgOut;
  if (myChannel.update ())
  { 
    Debug.print ("IN: ");
    Debug.write (myChannel.getData (), myChannel.getLength ()); 
    Debug.println ();
    for ( uint8_t i = 0; i < min( sizeof (msgIn), myChannel.getLength ()); i++) {
      msgIn.bytes[i] = myChannel.getData()[i];
    }
    digitalWrite (13        , HIGH);
    Debug.println ( msgOut.fromDevice );
    Debug.println ( msgOut.dataPoint  );
    Debug.println ( msgOut.value      );
    delay(100);
    digitalWrite (13        , LOW );
  }
  if ( millis() - lastMsgRecived > w) {
    Debug.println ("OUT: ");
    msgOut.fromDevice = 1;
    msgOut.dataPoint  = 2;
    msgOut.value      = 3;
  digitalWrite  (ENABLE_PIN, HIGH );
    myChannel.sendMsg (msgOut.bytes, sizeof (msgOut.bytes));
    lastMsgRecived = millis();
  digitalWrite  (ENABLE_PIN, LOW );
  }
}  // end of loop


