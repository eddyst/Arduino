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
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println( F("Start"));
  pinMode       (13        , OUTPUT);  // LED
  digitalWrite  (13        , LOW );
  // set the data rate for the SoftwareSerial port
  mySerial.begin();
//  mySerial.println("Hello, world?");
}
uint32_t zp;
uint8_t x;

void loop() // run over and over
{ uint8_t r; 
  if (mySerial.available()) {
    Serial.println("available");
    delay(10);
    Serial.write(mySerial.read());
  }  
  
// if (Serial.available())
//    mySerial.write(Serial.read()); 
  if ( millis() > zp + 1000) {
    zp = millis();
    Serial.print( ("zp="));
    Serial.print(zp);
    msgType msg;
    msg.id = ++x;
    msg.value = 35000;
    Serial.print("+");
    mySerial.write( msg.bytes, sizeof(msg.bytes));
    Serial.println('.');  
  }
    delay(100);
}


