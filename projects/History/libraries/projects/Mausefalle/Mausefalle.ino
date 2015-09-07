const int PinIrEnabled = 2;     // the number of the pushbutton pin
const int PinIrInput = A0;     // the number of the pushbutton pin
uint8_t irMaxDiv2 = 511; 
const int PinMotor = 5;

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(57600);
  // initialize the digital pin as an output.
  pinMode(PinIrEnabled, OUTPUT);     
  pinMode(PinIrInput  , INPUT);
  pinMode(PinMotor    , OUTPUT);
  pinMode(13          , OUTPUT);
  digitalWrite(PinIrEnabled, LOW);
  digitalWrite(PinMotor    , HIGH);

  Serial.print("irMaxDiv2=");
  Serial.println (irMaxDiv2);
  for (uint8_t i=0; i<100; i++) {
    int reading = analogRead(PinIrInput);
    if (reading < irMaxDiv2 * 4)
      irMaxDiv2 = reading / 4;
    delay(10);
    Serial.print (reading); 
    Serial.print(" ");
  }
  Serial.println ();
  Serial.print("irMaxDiv2=");
  Serial.println (irMaxDiv2);
}

uint8_t State = 0;
void loop(){
  int reading;
  //  Serial.println(irMaxDiv2);
  switch (State) {
  case 0:
    reading = analogRead(PinIrInput);
    if (reading < irMaxDiv2 * 4 - 30){
      Serial.println(" ");
      Serial.print("reading=");
      Serial.println(reading);
      digitalWrite(PinMotor    , LOW);
      delay(1000);
      digitalWrite(PinMotor    , HIGH);
      State=1;
      Serial.println("State=1");
    }
    break;
  case 1:
    digitalWrite(13, !digitalRead(13));
    delay(500);
  }
}


