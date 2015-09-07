#include <Bounce2.h>
#define DimDownIntervallMicros 10000 //= 25/Sec
#define DimDownSek 30
   #define DimDownStep DimDownIntervallMicros / (DimDownSek * 1000000 / DimDownIntervallMicros)

#define pinTaster1 A2
#define pinTaster2 A1
#define pinLampe1  9
#define pinLampe2  8
#define statAus          0
#define statSetAus       1
#define statAnTaster    10
#define statAnEth       20
#define statDim         30
#define statSetDim      31

uint8_t statusLampe1 = statAus;
uint16_t Lampe1 = 0;
uint32_t timLampe1;
uint32_t timLampe1PWM;
Bounce Taster1 = Bounce();

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin (115200);
  pinMode(pinTaster1, INPUT);
  pinMode(pinTaster2, INPUT);
  Taster1.attach(pinTaster2);
  Taster1.interval(50);

  pinMode(pinLampe1, OUTPUT);
  pinMode(pinLampe1, OUTPUT);
  
}

void loop() {
  Taster1.update ( );
  if( Taster1.rose()){
    Serial.println(F("Taster1.rised"));
    switch ( statusLampe1) {
    case statAus:
    case statDim:
      timLampe1 = millis();
      digitalWrite( pinLampe1, HIGH);
      statusLampe1 = statAnTaster;
      break;
    default:
      statusLampe1 = statSetAus;
    }
  };
  switch ( statusLampe1) {
  case statSetAus:
    Serial.println(F("statSetAus"));
    digitalWrite( pinLampe1, LOW);
    statusLampe1 = statAus;
    break;
  case statAus:
    break;
  case statAnTaster:
    if (millis() - timLampe1 > 3000) {
      Serial.println(F("statAnTaster abgelaufen"));
      statusLampe1 = statSetDim;
    }
    break;
  case statAnEth:
    if (millis() - timLampe1 > 300000) {
      Serial.println(F("statAnEth abgelaufen"));
      statusLampe1 = statSetDim;
    } else
      if (digitalRead( pinLampe1))
        if (micros() - timLampe1PWM > Lampe1)
          digitalWrite( pinLampe1, LOW);
      else 
        if (micros() - timLampe1PWM > DimDownIntervallMicros) {
          timLampe1PWM = micros();
          digitalWrite( pinLampe1, HIGH);
        }  
    break;
  case statSetDim:
    Serial.println(F("statSetDim"));
    timLampe1PWM = micros();
    Lampe1 = DimDownIntervallMicros;
    Serial.println(DimDownStep);
    statusLampe1 = statDim;
    break;
  case statDim:
   if (Lampe1 < DimDownStep) {
      Serial.println(F("statDim abgelaufen"));
      statusLampe1 = statSetAus;
    } else {
      if (digitalRead( pinLampe1)) {
        //Serial.println("pin = H");
        if (micros() - timLampe1PWM > Lampe1) {
//          Serial.println(F("L"));
          digitalWrite( pinLampe1, LOW);
        }
      }
      else {
 /*       Serial.print(timLampe1PWM);
          Serial.print(F(","));
        Serial.print(micros() - timLampe1PWM);
          Serial.print(F(","));
        Serial.println(DimDownIntervallMicros);
*/
        if (micros() - timLampe1PWM > DimDownIntervallMicros) {
          timLampe1PWM = micros();
          digitalWrite( pinLampe1, HIGH);
          Lampe1 -= DimDownStep;
//          Serial.println(Lampe1);
        }
      }
    }
    break;
  }
}
void statSetAnEth() {
    Serial.println(F("statSetAn..."));
    timLampe1 = millis();
    timLampe1PWM = micros();
    statusLampe1 = statAnEth;
}
