#define pinOben 8
#define pinUnten 9
#define pinMotorHoch 4
#define pinMotorRunter 3
#define pinMotorEnabled 5

void setup() {
  // put your setup code here, to run once:
#ifdef Debug
  Serial.begin(115200);
#endif
  pinMode(pinOben, INPUT);
  pinMode(pinUnten, INPUT);
  pinMode(A1,INPUT);
  pinMode(pinMotorHoch, OUTPUT);
  pinMode(pinMotorRunter, OUTPUT);
  analogWrite(pinMotorEnabled, 0);
  digitalWrite(pinMotorHoch,LOW);
  digitalWrite(pinMotorRunter,LOW);
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
}

#define StateOben 1
#define StateHoch 2
#define StateRunter 3
#define StateUnten 4
uint8_t State=0;

void loop() {
  // put your main code here, to run repeatedly:
//  if (State != StateOben && digitalRead(pinOben) == HIGH)
#ifdef Debug
  Serial.print(millis());
#endif  
  uint16_t Schalter = analogRead(A1);
#ifdef Debug
  Serial.println(Schalter);
#endif
  if (Schalter <= 740){
    digitalWrite(pinMotorHoch, HIGH);
    digitalWrite(pinMotorRunter, LOW);
    analogWrite(pinMotorEnabled, 255);
#ifdef Debug
    Serial.println("Hoch");
#endif
  } else if (Schalter <= 850) {
    digitalWrite(13,HIGH);
    digitalWrite(pinMotorHoch, LOW);
    digitalWrite(pinMotorRunter, HIGH);
    analogWrite(pinMotorEnabled,150);
#ifdef Debug
    Serial.println("Runter");
#endif
  } else {
     digitalWrite(pinMotorHoch, LOW);
     digitalWrite(pinMotorRunter, LOW);
   analogWrite(pinMotorEnabled, 0);
#ifdef Debug
    Serial.println("AUS");
#endif
  }
  delay(150);
}
