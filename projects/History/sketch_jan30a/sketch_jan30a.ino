#include <Wire.h>
void setup() {
  // put your setup code here, to run once:
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
}

void loop() {
  // put your main code here, to run repeatedly:



  Wire.requestFrom(0x78,2); // read 2 bytes
  if(Wire.available()==2) {
    uint8_t Th = Wire.read(); // receive 1st byte
    uint8_t Tl = Wire.read(); // receive 2nd byte
    double val = double((Th<<8 | Tl))/128.0 - 32.0;
    Serial.print(Th);
    Serial.print(" - ");
    Serial.print(Tl);
    Serial.print(" = ");
    Serial.println(val);
  }
delay(500);  

}
