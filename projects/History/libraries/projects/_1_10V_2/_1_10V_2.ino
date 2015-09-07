int led = 3;           // the pin that the LED is attached to
int brightness = 20;    // how bright the LED is

void setup()  { 
  Serial.begin(57600);
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
} 

void loop()  { 
  while (Serial.available() > 0) {
    Serial.print("A: ");
    // look for the next valid integer in the incoming serial stream:
    brightness = Serial.parseInt(); 

    // set the brightness of pin 9:
  //  if (Serial.read() == '\n') {
      Serial.print("Set:");
      analogWrite(led, brightness);    
 //   }
    Serial.println(brightness);

  }                        
}


