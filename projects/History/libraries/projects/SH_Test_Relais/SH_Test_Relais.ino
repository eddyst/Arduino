/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// the setup routine runs once when you press reset:
void setup() {                
  for (int i = 23;i<38;i+=2) {
  // initialize the digital pin as an output.
  pinMode(i, OUTPUT);     
  }
  pinMode(34, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  for (int i = 23;i<38;i+=2) {
digitalWrite(34, HIGH);   // turn the LED on (HIGH is the voltage level)
  
//  digitalWrite(i, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(34, LOW);   // turn the LED on (HIGH is the voltage level)
  delay (1000);
//  digitalWrite(i, LOW);    // turn the LED off by making the voltage LOW
  }
}
