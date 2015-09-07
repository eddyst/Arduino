void DoWorkInit() {
  pinCount=sizeof(RelayPins)/sizeof(int);
  for (int i=0;i<pinCount;i++)
  { 
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i],LOW);
  }
}
void DoWork()
{ 
  static unsigned long LastDoWork;
  static unsigned long LastCheckAll;
  static byte Status=0;
  if (millis() - LastDoWork > 15000) {
    Serial.print(F("(Status/2)=")); 
    Serial.print((Status/2));
    Serial.print(F("  %  (pinCount)=")); 
    Serial.println((pinCount));
    byte Pin = (Status/2)%(pinCount);
    Serial.print(F("DoWork       Status ="));  
    Serial.print(Status); 
    Serial.print(F(" Pin ="));  
    Serial.print(Pin);
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
      LastDoWork = millis() ;
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
        LastValue[Pin] = sensorValue;
        bitSet(Changed,Pin);
      }
      if (Status  < pinCount * 2 )
      { 
        Serial.println(F(" Status ++"));
        Status ++;
      }
      else
        if (millis() - LastCheckAll > 600000)      { 
          LastCheckAll = millis();
          Serial.println(F(" Status = 0"));
          Status = 0;
        }
        else      { 
          Serial.println(F(" SetLastWork"));
          LastDoWork = millis();
        }
      break;
      //default: 
    }
    Serial.print(F("             Status ="));  
    Serial.print(Status); 
    Serial.print(F("  millis()="));  
    Serial.println(millis());
    Serial.print(F("LastDoWork="));  
    Serial.println(LastDoWork);
  }
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
