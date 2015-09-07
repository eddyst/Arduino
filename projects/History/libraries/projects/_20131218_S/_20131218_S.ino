volatile int c1 = 0;
#define ledPin 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("Start");

  attachInterrupt(0, blink, FALLING);

  pinMode(ledPin, OUTPUT);
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  OCR2A = 150;              // compare match register 16MHz/256/2Hz
  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2B |= (1 << CS21)              ; //    8 prescaler 
//TCCR2B |= (1 << CS21) | (1 << CS20); //   64 prescaler 
//TCCR2B |= (1 << CS22)              ; //  256 prescaler 
//TCCR2B |= (1 << CS22) | (1 << CS20); // 1024 prescaler 
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER2_COMPA_vect)          // timer compare interrupt service routine
{
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
}

uint32_t t;
void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - t > 1000) {
    t=millis();
    Serial.println(c1);
  }
}

void blink()
{
    digitalWrite(ledPin, HIGH);
    for (uint32_t i = 0; i<100000; i++) {
      ;
    }
    digitalWrite(ledPin, LOW);
    c1++;
}
