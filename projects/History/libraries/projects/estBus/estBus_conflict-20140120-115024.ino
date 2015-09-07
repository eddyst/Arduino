#include <Arduino.h>
#include <estBUS.h>

#define rxPin 10
#define txPin 11

typedef union {
  uint8_t bytes[5];
  struct {
    uint8_t id;
    int32_t value;
  };
}
msgType;

estBUS mySerial(rxPin, txPin); // RX, TX

void setup()
{ 
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  mySerial.begin();
//  mySerial.println("Hello, world?");
}

void loop() // run over and over
{ uint8_t r; 
  if (mySerial.available()) {
    Serial.print("->");
    Serial.println(mySerial.read());
  }  
}


