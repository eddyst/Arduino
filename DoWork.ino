void DoWorkInit() {
  if (pinCount!=sizeof(SensorPins)/sizeof(int)) {
    Debug.println(F("pinCount WEICHT VON SensorPins AB"));
  }
  for (int pin=0;pin<pinCount;pin++) { 
    LastTemp [pin] = 1258;
    LastValue[pin] = 1258;
    sample   [pin] = 0;
    pinMode(SensorPins[pin], INPUT);
    digitalWrite(SensorPins[pin], LOW);
  }
}
void DoWork() { 
  static unsigned long LastDoWork;
  static int i = 0;
  
  if (micros() - LastDoWork > (Intervall * 1000 / 60)) {
    #if aDCLogLevel > 1
      Debug.print(i); 
      Debug.print(F(": ")); 
    #endif 
    if (i++ == 60) {
      i = 1;
      for (int pin=0; pin < pinCount; pin++) {
        sample[pin] = sample[pin] / 60;  //get the average of 60 samples
        #if aDCLogLevel > 0
          Debug.print(F(" Index "));  
          Debug.print(pin);
          Debug.print(F(" ADC=")); 
          Debug.print(sample[pin]);
          Debug.print(F(" LastValue=")); 
          Debug.print(LastValue[pin]);
        #endif
        if ( abs(LastValue[pin] - sample[pin]) > 1) {
          LastValue[pin] = sample[pin];
          bitSet(Changed,pin);
          if (LastValue[pin] < 400) {
            LastTemp[pin] = ((int32_t)LastValue[pin] * 15 - 1620) / 10;
          } else {
            LastTemp[pin] = ((int32_t)LastValue[pin] * 19 - 3320) / 10;
          }
          #if aDCLogLevel > 0
            Debug.print(F(" --> Temp="));  
            Debug.print(LastTemp[pin]); 
          #endif
        }
        sample[pin] = 0;
        #if aDCLogLevel > 0
          Debug.println();
          Debug.print(F("0: "));  
        #endif
      }
    }
    #if aDCLogLevel > 3
      Debug.print(F(" Pin: "));
    #endif
    for (int pin=0; pin < pinCount; pin++) {
      int v = analogRead( SensorPins[pin]);
//      int v = 1023;
      sample[pin] += v;  //get 60 samples added together 1024 x 60 max
      #if aDCLogLevel > 3
         Debug.print(v);  
         Debug.print(F(" sample["));
         Debug.print(pin);
         Debug.print(F("]="));
         Debug.print(sample[pin]);
         Debug.print(F(" "));
      #endif
    }
    LastDoWork = micros();
    LastDoWork -= LastDoWork % (Intervall * 1000 / 60);
    #if aDCLogLevel > 1
      Debug.print(F("    LastDoWork -> ")); 
      Debug.println(LastDoWork);
    #endif
  }
}


