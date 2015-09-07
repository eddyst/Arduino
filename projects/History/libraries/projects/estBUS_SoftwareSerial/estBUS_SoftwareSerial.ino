#include <SoftwareSerial.h>
#include <estBUS.h>

SoftwareSerial sSerial (2, 3);  // receive pin, transmit pin
const byte ENABLE_PIN = 4;

int fAvailable ()
{
  return sSerial.available ();
}

int fRead ()
{
  return sSerial.read ();
}

size_t fWrite (const byte what)
{
  sSerial.write (what);
}

estBUS myChannel (fRead, fAvailable, fWrite, fPrepareAnswer, fMessageRecived, 20);


typedef union {
  byte bytes[4];
  struct {
    int32_t value;
  };
}
msgType;

void setup ()
{
  Serial.begin  (115200);
  Serial.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  pinMode       (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite  (ENABLE_PIN, LOW );
  sSerial.begin (28800);
  myChannel.begin ();
  randomSeed(analogRead(0));
}  // end of setup

void loop ()
{
  //  myChannel.sendMsg (msg, sizeof (msg));
  msgType msg;
  if (myChannel.update ())
  {
    Serial.print ("IN: ");
    Serial.write (myChannel.getData (), myChannel.getLength ());
    Serial.println ();
    for ( uint8_t i = 0; i < min( sizeof (msg), myChannel.getLength ()); i++) {
      msg.bytes[i] = myChannel.getData()[i];
    }
    //   switch case (msg.
    digitalWrite (13        , HIGH);
    delay(100);
    digitalWrite (13        , LOW );
  }
}  // end of loop

void fPrepareAnswer( const byte TimeSlotID, byte* data, byte dataLength) {
  msgType msg;
  msg.value = 35000;
  for ( uint8_t i = 0; i < 4; i++) {
    data[i] = msg.bytes[i];
   }
  dataLength = 4;
}

void fMessageRecived( const byte TimeSlotID, byte* data, byte dataLength) {
  msgType msg;
  for ( uint8_t i = 0; i < 4; i++) {
    msg.bytes[i] = data[i];
  }
  Serial.print ("IN: ");
  Serial.write (myChannel.getData (), myChannel.getLength ());
  Serial.print (" -> ");
  Serial.write ( msg.value);
  Serial.println ();
}

