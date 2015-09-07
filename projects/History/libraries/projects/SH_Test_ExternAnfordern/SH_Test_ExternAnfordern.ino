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
  pinMode(30, OUTPUT);  
  pinMode( 4, OUTPUT);     
  pinMode(24, OUTPUT);     
  pinMode(26, OUTPUT);  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(30, HIGH);   // Happytemp anschalten
  digitalWrite(24, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(26, HIGH);   // turn the LED on (HIGH is the voltage level)
  analogWrite ( 4, 128 );
}
