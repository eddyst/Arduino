void DoWorkInit() {
  pinMode(PowerPin, OUTPUT);
  digitalWrite(PowerPin,LOW);
  if (pinCount!=sizeof(SensorPins)/sizeof(int)) {
    Debug.print(F("pinCount WEICHT VON SensorPins AB"));
  }
  for (int i=0;i<pinCount;i++)
  { 
    pinMode(SensorPins[i], INPUT);
    digitalWrite(SensorPins[i], LOW);
  }
}
void DoWork() { 
  static unsigned long LastDoWork;
  static byte Status=0;
  switch (Status) { 
  case 0:
    if (millis() - LastDoWork > 1000) {
      Debug.println();
      Debug.print(F("PowerPin > HIGH")); 
      digitalWrite(PowerPin, HIGH);
      Status ++;
    }
    break;
  case 1:
    if (millis() - LastDoWork > 2000) {
      Debug.println();
      unsigned long ms;
      unsigned int sample[pinCount - 1];   //reset the sample value
      for (int pin=0; pin<pinCount; pin++) {
        sample[pin] = 0;
      }
      for(int i = 0; i<60; i++) {
        ms = micros();
        #if aDCLogLevel > 3
           Debug.print(ms);
           Debug.print(F(" Pin: "));
        #endif
        for (int pin=0; pin<pinCount; pin++) {
          int v = analogRead( SensorPins[pin]);
          sample[pin] += v;  //get 60 samples added together 1024 x 60 max
          #if aDCLogLevel > 3
             Debug.print(v);  
             Debug.print(F(" "));
          #endif
        }
        int z=0; 
        while (micros() - ms < 2000) {
          z++; 
        }
        #if aDCLogLevel > 2
          Debug.print(F("z = "));
          Debug.println(z);
        #endif
      } 
      for (int pin=0; pin<pinCount; pin++) {
        int32_t sensorValue = sample[pin] / 60;  //get the average of 60 samples
        #if aDCLogLevel > 0
          Debug.print(F(" pin "));  
          Debug.print(pin);
        #endif
        #if aDCLogLevel > 1
          Debug.print(F(" SensorValue=")); 
          Debug.print(sensorValue);
        #endif
        LastValue[pin] = sensorValue;
        sensorValue = (-75000 + (sensorValue - 25) * 1505) / 1000;
        #if aDCLogLevel > 1
          Debug.print(F(" --> "));  
          Debug.print(sensorValue); 
          Debug.print(F(" | "));  
          Debug.print(LastTemp[pin]);
        #endif
        if ( abs(LastTemp[pin]-sensorValue) > 1) {
          #if aDCLogLevel > 0
            Debug.println(F(" --> Set LastTemp")); 
          #endif
          LastTemp[pin] = sensorValue;
          bitSet(Changed,pin);
        }
      }
      #if aDCLogLevel > 1
        Debug.println();
        Debug.println(F("PowerPin > LOW")); 
      #endif
      digitalWrite(PowerPin, LOW);
      Status ++;
    }
    break;
  default: 
#define Intervall 10000
    if (millis() - LastDoWork > Intervall) {
       #if aDCLogLevel > 1
        Debug.print(F("LastDoWork")); 
        Debug.print(LastDoWork);
        Debug.print(F(" - ")); 
        Debug.print(LastDoWork % Intervall);
      #endif
      LastDoWork = millis() - LastDoWork % Intervall;
      #if aDCLogLevel > 1
        Debug.print(F(" > ")); 
        Debug.println(LastDoWork);
      #endif
      Status = 0;
    }
  }
}


