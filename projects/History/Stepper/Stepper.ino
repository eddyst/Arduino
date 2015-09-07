const int buttonPin = 2;     // the number of the pushbutton pin
int dirPin = 4;
int stepperPin = 5;

int d1;
int d2;
int lastButtonState = LOW;   // the previous reading from the input pin

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(buttonPin, INPUT);

  pinMode(13, OUTPUT);     
  pinMode(dirPin, OUTPUT);
  pinMode(stepperPin, OUTPUT);
}

void step(boolean dir,int steps){
  digitalWrite(dirPin,dir);
  delay(50);
  for(int i=0;i<steps;i++){
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(200);
    digitalWrite(stepperPin, LOW);
    delayMicroseconds(200);
  }
}
boolean oo;
void loop(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  if (reading == lastButtonState){
    if (oo) {
      step(true,800);
      delay(500);
      step(false,1600*1);
      delay(500);
    }
  }
  else{
    lastButtonState = reading;
    if (reading == LOW){
      oo=!oo;
    }
  }
  digitalWrite(13, !reading);
}
/*
void loop() {
 if (d1>0){
 d1=0;
 digitalWrite(4, HIGH);   
 } 
 else {
 d1=1;
 digitalWrite(4, LOW);   
 }
 for (int i=100;i-=5;i>1) {
 if (d2>0){
 d2=0;
 digitalWrite(13, HIGH);   
 } 
 else {
 d2=1;
 digitalWrite(13, LOW);   
 }
 for (int s=(101-i)*2;s--;s>1){
 digitalWrite(5, HIGH); 
 delay(1);  
 digitalWrite(5, LOW);    
 delay(i);
 }
 }
 }
 
 */

