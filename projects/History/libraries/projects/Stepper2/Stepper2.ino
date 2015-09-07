const int PinSleep=4;
const int PinDirection = 5;
const int PinDoStep = 6;

void setup() {
  Serial.begin(57600);
  pinMode(PinSleep    , OUTPUT);
  pinMode(PinDirection, OUTPUT);
  pinMode(PinDoStep   , OUTPUT);
  digitalWrite( PinSleep, LOW);
}

uint32_t s = 800000, w=120, st=0;
void loop() {
/*  // put your main code here, to run repeatedly: 
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    Serial.print(" ");
    Serial.println(inChar,DEC);
    switch (inChar){
    case 111: //o
      w+=10;
      break;
    case 112: //p
      s+=1;
      break;
    case 110: //n
      w-=10;
      break;
    case 109: //m
      st-=1;
      break;
    }
  }
  */
  Serial.print(s);    Serial.print(" ");
    Serial.println(w);

  digitalWrite( PinDirection, HIGH);
  step(s,w,st);
  delay(1000);
  digitalWrite( PinDirection, LOW);
  step(s,w,st);
  delay(1000);
  
}

void step(uint32_t Steps, uint32_t Wait, uint16_t s) {
  digitalWrite( PinSleep, HIGH);
  for(uint32_t i=0;i<Steps;i++){
    digitalWrite(PinDoStep, LOW);
    digitalWrite(PinDoStep, HIGH);
    int x = Wait - i*s;
    delayMicroseconds(x);
  }
  digitalWrite( PinSleep, LOW);
}
/*
const uint16_t stepTime = 1200;
int Pos = 0;
uint32_t lastStepMicros;
void Move(int toPos) {
  Pos += toPos;
  digitalWrite( PinSleep, HIGH);
  digitalWrite( PinMotor, Pos>0);
  lastStepMicros = micros() - stepTime + 50;
}
void DoStep(){
  if ( Pos!=0 && micros() - lastStepMicros >= stepTime) {
    digitalWrite(PinDoStep, LOW);
    digitalWrite(PinDoStep, HIGH);
    lastStepMicros = micros();
  }
}
*/
