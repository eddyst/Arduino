#include <MemoryFree.h> //from www.arduino.cc/playground/Code/AvailableMemory

String Log=String("");
uint8_t i = 0;

void setup() {
  delay(10000);           //So I have time to open the Serial Monitor
  Serial.begin(57600);
  pinMode(13,OUTPUT);
}

void loop() {
  String B = String ( "");
  if ( i % 2 == 1)            // Blink to see if it is running
    digitalWrite( 13, HIGH); 
  else
    digitalWrite( 13, LOW);   
//  Serial.print (i, HEX);     // awaiting
//  Serial.print (" - ");
  B = toHex(i);
  Serial.print   ( B);          // current output
  Serial.print   ( " - ");
  Serial.print   ( freeMemory(), DEC);  // Memory
  Serial.print   ( " - ");
  Log += String( B);
  Serial.println ( Log);
  i++;
  delay(500);
}

String toHex (uint8_t value) {
  if ( value < 16)
    return String("0") + String(value, HEX);
  else
    return String(value, HEX);
}


/*
Output:

00 - 1789 - 00
01 - 1788 - 0001
02 - -23342 - 000102
03 - -23344 - 00010203
04 - -24626 - 0001020304
+5 - -25816 - 0001020304+5
&6 - 4305 - 0001020304+5&6
&7 - -3015 - 0001020304+5&6&7
00 - 1789 - 00
01 - 1788 - 0001
*/
// After this it stops printing and blinking
