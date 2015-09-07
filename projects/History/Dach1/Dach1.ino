#include <EtherCard.h>

int sensorPin=12;
int RelayPins[] = { 
  0, 1, 2, 3 };       // an array of pin numbers to which Relays are attached
//  3, 2, 1, 0 };       // an array of pin numbers to which Relays are attached
int pinCount;

unsigned long NextDoWork;

void setup(void){ 
  Serial.begin(57600);
  Serial.println(F("---Setup---"));
  pinMode(sensorPin, INPUT);
  pinCount=sizeof(RelayPins)/sizeof(int);
  for (int i=0;i<pinCount;i++)
  { 
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i],LOW);
  }
  delay(2000);
  // Zum kalibrieren
  /*
 for (;;)
   { digitalWrite(0,HIGH); digitalWrite(3,LOW);
   delay(1000);
   Serial.print(analogRead(sensorPin));
   digitalWrite(0,LOW); digitalWrite(3,HIGH);
   delay(1000);
   Serial.println(analogRead(sensorPin));
   }
   */
  //  Serial.println(ether.doBIST(28));
  EthInit();
  Serial.println(F("---Setup End---"));
}

void loop(void){
  ethDoEvents();
  if (millis()>=NextDoWork)
  { 
    DoWork();
  }
  return;
}

void DoWork()
{ 
  static unsigned long NextCheckAll;
  static byte Status=0;
  static int LastValue[4];
  Serial.print(F("(Status/2)=")); 
  Serial.print((Status/2));
  Serial.print(F("  %  (pinCount)=")); 
  Serial.println((pinCount));
  byte Pin = (Status/2)%(pinCount);
  Serial.print(F("DoWork       Status ="));  
  Serial.print(Status); 
  Serial.print(F(" Pin ="));  
  Serial.print(Pin);
  if (Status == 0)
    NextCheckAll = millis() + 600000;
  switch (Status % 2) 
  { 
  case 0:
    Serial.println();
    for (int i=0; i<pinCount;i++)
      if (i==Pin)
      { 
        Serial.print(F("<")); 
        Serial.print(RelayPins[i]); 
        Serial.print(F(">"));
        digitalWrite(RelayPins[i], HIGH);
      }
      else
      { 
        Serial.print(F(" ")); 
        Serial.print(RelayPins[i]); 
        Serial.print(F(" ")); 
        digitalWrite(RelayPins[i], LOW);
      }
    Serial.println();
    Serial.println(F(" Status ++"));
    Status ++;
    NextDoWork = millis()+15000;
    break;
  case 1:
    // read the value from the sensor:
    int sensorValue = adc(sensorPin);
    Serial.print(F(" SensorValue=")); 
    Serial.print(sensorValue);
    sensorValue = (sensorValue * 48828 - 7857143) * 35 / 1000000;
    Serial.print(F(" --> "));  
    Serial.print(sensorValue); 
    Serial.print(F(" | "));  
    Serial.println(LastValue[Pin]);

    if (Status < pinCount * 2 or abs(LastValue[Pin]-sensorValue) > 1)
    { 
      VZLog(Pin,sensorValue);
      LastValue[Pin] = sensorValue;
    }
    if (Status  < pinCount * 2 )
    { 
      Serial.println(F(" Status ++"));
      Status ++;
    }
    else
      if (millis() > NextCheckAll)      { 
        Serial.println(F(" Status = 0"));
        Status = 0;
      }
      else      { 
        Serial.println(F(" SetNextWork"));
        NextDoWork = millis()+20000;
      }
    break;
    //default: 
  }
  Serial.print(F("             Status ="));  
  Serial.print(Status); 
  Serial.print(F("  millis()="));  
  Serial.println(millis());
  Serial.print(F("NextDoWork="));  
  Serial.println(NextDoWork);
}

inline unsigned int adc(int pin)
{
 unsigned long ms;
 unsigned int sample = 0;   //reset the sample value
 Serial.println();
 for(int i = 0; i<60; i++) {
   ms=micros();
   int v = analogRead(pin);
   sample += v;  //get 60 samples added together 1024 x 60 max
   Serial.print(ms);  
   Serial.print(F(" "));
   Serial.print(v);  
   Serial.print(F(" "));
   int z=0; 
   while (micros() - ms < 1100) {
     z++; Serial.print(F("."));
   }
   Serial.println(z);
 } 
 return sample / 60;  //get the average of 60 samples
}//end of adc
