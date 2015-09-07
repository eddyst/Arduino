#define PinSleep        4
#define PinDirection    5
#define PinDoStep       6

#define dirWaermer LOW
#define dirKaelter !dirWaermer


void initStepper() {
  pinMode(PinSleep      , OUTPUT);
  digitalWrite( PinSleep, LOW);
  pinMode(PinDirection  , OUTPUT);
  pinMode(PinDoStep     , OUTPUT);
TCCR2B = _BV( CS21);  //Prescaler = 8
TCCR2A = _BV( WGM21); //set WGM to CTC mode (010) : timer 2 counts until it matches OCR2A
OCR2A  = 240;          // = 1µs / (16MHz / Prescaler8 / 240) = 120µs
}

#define TimerStart TIMSK2 = _BV( OCIE2A); digitalWrite( PinSleep, HIGH);

#define TimerStop  TIMSK2 = 0; digitalWrite( PinSleep, LOW);

void setPos (uint32_t newPos) {
  Debug.print  ( F( " setPos "));
  Debug.println( newPos);
  posSoll = newPos;
  if ( posSoll > pos) {                         // Soll wärmer werden
    digitalWrite( PinDirection, dirWaermer);
    TimerStart
  }
  else if (posSoll < pos) {
    digitalWrite( PinDirection, dirKaelter);
    TimerStart
  }
  Debug.println (digitalRead( PinDirection));
}     

ISR(TIMER2_COMPA_vect) {
  if ( posSoll > pos) {                         // Soll wärmer werden
    if ( bitRead( kali, KaliWarm)) { //Schrittfehler
      SFehlerMax = Schritte - pos; 
      Schritte = pos;
      posSoll = pos;
    }
    else {
      pos++;
      digitalWrite( PinDoStep, HIGH);
      digitalWrite( PinDoStep, LOW);
    }
  }
  else if (posSoll < pos) {
    if ( bitRead( kali, KaliKalt)) { //Schrittfehler
      SFehlerMin = pos;
      Schritte += pos;
      pos = 0;
      posSoll = pos;
    }
    else {
      pos--;
      digitalWrite( PinDoStep, HIGH);
      digitalWrite( PinDoStep, LOW);
    }
  } 
  else if (pos == 0 && !bitRead( kali, KaliKalt)) {// Schrittfehler Auf dem Weg zu 0 sind Schritte verloren gegangen
    if ( SFehlerMin > 0) 
      SFehlerMin = -1;
    else
      SFehlerMin--;
    digitalWrite( PinDoStep, HIGH);
    digitalWrite( PinDoStep, LOW);
    Schritte++;
  } 
  else if (pos == Schritte && !bitRead( kali, KaliWarm)) {// Schrittfehler Auf dem Weg zu 0 sind Schritte verloren gegangen
    if ( SFehlerMax > 0) 
      SFehlerMax = -1;
    else
      SFehlerMax--;
    digitalWrite( PinDoStep, HIGH);
    digitalWrite( PinDoStep, LOW);
    Schritte++;
  }  
  else {
    TimerStop
  }
}

