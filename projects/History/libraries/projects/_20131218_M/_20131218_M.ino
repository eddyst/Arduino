void setup() {
  // put your setup code here, to run once:
  digitalWrite(3,HIGH);
  pinMode(3, OUTPUT);
  delay(2000);
  for ( int i=0 ; i<10; i++) {
  digitalWrite(3,HIGH);
  delayMicroseconds(1000);
  digitalWrite(3,LOW);
  delayMicroseconds(50);
  }
  digitalWrite(3,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
//digitalWrite(3,HIGH);
//delayMicroseconds(150);
//digitalWrite(3,LOW);
//delayMicroseconds(50);
//digitalWrite(3,HIGH);
//delay(10);
//digitalWrite(3,LOW);
//delay(10);
}
