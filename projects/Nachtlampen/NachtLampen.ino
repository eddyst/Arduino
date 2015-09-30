#include <Bounce2.h>
#define DimDownIntervallMicros 10000 //= 25/Sec
#define DimDownSek 30
#define DimDownStep DimDownIntervallMicros / (DimDownSek * 1000000 / DimDownIntervallMicros)

struct typLampe {
  uint8_t pinTaster;
  Bounce Taster = Bounce();
  uint8_t pinLampe;
  uint8_t statusLampe;
  uint16_t LampePWMWert;
  uint32_t timLampe;
  uint32_t timLampePWM;
};
typLampe Lampe[2];

#define statAus          0
#define statSetAus       1
#define statAnTaster    10
#define statAnEth       20
#define statDim         30
#define statSetDim      31


// the setup routine runs once when you press reset:
void setup() {
  Lampe[0].pinTaster = A2;
  Lampe[1].pinTaster = A1;
  Lampe[0].pinLampe = 9;
  Lampe[1].pinLampe = 8;

  Serial.begin (115200);
  for (uint8_t i = 0; i<2; i++) {
    pinMode(Lampe[i].pinTaster, INPUT);
    Lampe[i].Taster.attach(Lampe[i].pinTaster);
    Lampe[i].Taster.interval(50);
    pinMode(Lampe[i].pinLampe, OUTPUT);
    Lampe[i].statusLampe = statAus;
  }
  
}

void loop() {
  for (uint8_t i = 0; i<2; i++) {
    Lampe[i].Taster.update ( );
    if( Lampe[i].Taster.rose()){
      Serial.println(F("Lampe[i].Taster.rised"));
      switch ( Lampe[i].statusLampe) {
      case statAus:
      case statDim:
        Lampe[i].timLampe = millis();
        digitalWrite( Lampe[i].pinLampe, HIGH);
        Lampe[i].statusLampe = statAnTaster;
        break;
      default:
        Lampe[i].statusLampe = statSetAus;
      }
    };
    switch ( Lampe[i].statusLampe) {
    case statSetAus:
      Serial.println(F("statSetAus"));
      digitalWrite( Lampe[i].pinLampe, LOW);
      Lampe[i].statusLampe = statAus;
      break;
    case statAus:
      break;
    case statAnTaster:
      if (millis() - Lampe[i].timLampe > 3000) {
        Serial.println(F("statAnTaster abgelaufen"));
        Lampe[i].statusLampe = statSetDim;
      }
      break;
    case statAnEth:
      if (millis() - Lampe[i].timLampe > 300000) {
        Serial.println(F("statAnEth abgelaufen"));
        Lampe[i].statusLampe = statSetDim;
      } else
        if (digitalRead( Lampe[i].pinLampe))
          if (micros() - Lampe[i].timLampePWM > Lampe[i].LampePWMWert)
            digitalWrite( Lampe[i].pinLampe, LOW);
        else 
          if (micros() - Lampe[i].timLampePWM > DimDownIntervallMicros) {
            Lampe[i].timLampePWM = micros();
            digitalWrite( Lampe[i].pinLampe, HIGH);
          }  
      break;
    case statSetDim:
      Serial.println(F("statSetDim"));
      Lampe[i].timLampePWM = micros();
      Lampe[i].LampePWMWert = DimDownIntervallMicros;
      Serial.println(DimDownStep);
      Lampe[i].statusLampe = statDim;
      break;
    case statDim:
     if (Lampe[i].LampePWMWert < DimDownStep) {
        Serial.println(F("statDim abgelaufen"));
        Lampe[i].statusLampe = statSetAus;
      } else {
        if (digitalRead( Lampe[i].pinLampe)) {
          //Serial.println("pin = H");
          if (micros() - Lampe[i].timLampePWM > Lampe[i].LampePWMWert) {
  //          Serial.println(F("L"));
            digitalWrite( Lampe[i].pinLampe, LOW);
          }
        }
        else {
   /*       Serial.print(Lampe[i].timLampePWM);
            Serial.print(F(","));
          Serial.print(micros() - Lampe[i].timLampePWM);
            Serial.print(F(","));
          Serial.println(DimDownIntervallMicros);
  */
          if (micros() - Lampe[i].timLampePWM > DimDownIntervallMicros) {
            Lampe[i].timLampePWM = micros();
            digitalWrite( Lampe[i].pinLampe, HIGH);
            Lampe[i].LampePWMWert -= DimDownStep;
  //          Serial.println(Lampe[i].LampePWMWert);
          }
        }
      }
      break;
    }
  }
}
/*
void statSetAnEth(uint8_t i) {
    Serial.println(F("statSetAn..."));
    Lampe[i].timLampe = millis();
    Lampe[i].timLampePWM = micros();
    Lampe[i].statusLampe = statAnEth;
}
*/

