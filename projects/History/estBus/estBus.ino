#include <SoftwareSerial.h>
#include <estBUS.h>

#define rxPin 10
#define txPin 11
#define bufferSize 20
estBUS myChannel ( rxPin, txPin, bufferSize); //rxPin, txPin, bufferSize
uint32_t zp;

typedef union {
  uint8_t bytes[5];
  struct {
    uint8_t id;
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
//    Serial.write (myChannel.getData (), myChannel.getLength ());
    Serial.println ();
    for ( uint8_t i = 0; i < min( sizeof (msg), myChannel.getLength ()); i++) {
      msg.bytes[i] = myChannel.getData()[i];
    }
    //   switch case (msg.
    digitalWrite (13        , HIGH);
    delay(100);
    digitalWrite (13        , LOW );
  }
  if ( millis() - zp > 1000) {
    msgType msg;
    msg.id = 1;
    msg.value = 35000;
    myChannel.sendMsg( msg.bytes, sizeof(msg.bytes));
    zp = millis();
  }
}  // end of loop


