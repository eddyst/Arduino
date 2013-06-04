#define HappyTempPin 36
#define WWVentilPin 38
#define WWVentilStellungWarmwasser HIGH
#define WWVentilSperrenPin 40
#define ExternAnfordernPin 42
#define ThermeVorlaufTempVorgabePin 44

uint8_t WWControlState = 0;

void WarmwasserBegin(){
  if (wwLogLevel > 0) Debug.println ("WarmwasserBegin()");
  digitalWrite(HappyTempPin, HIGH);
  WWControlState=1;
}

void WarmwasserEnde(){
  if (wwLogLevel > 0) Debug.println ("WarmwasserEnde()");
  digitalWrite(HappyTempPin, LOW);
  WWControlState=0;
}

void ThermeVorlaufTempVorgabeRechnen() {
  if (wwLogLevel > 0) Debug.println ("\nThermeVorlaufTempVorgabeRechnen()");
  if (WWControlState == 4) {
    WWControlState = 3;
  }
}
void wwInit() {
  pinMode     (HappyTempPin, OUTPUT);
  digitalWrite(HappyTempPin, LOW);

  pinMode     (WWVentilPin , INPUT); 
  pinMode     (ThermeVorlaufTempVorgabePin, OUTPUT);
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
}

void   WWControlDoEvents() {
  static uint16_t lastThermeVorlaufTempVorgabeCheck;
  if (WWControlState >  0) {
    static uint16_t WWVentilOKSince;
    boolean WWVentilValue = digitalRead(WWVentilPin);
    if (Values[_ThermeUmschaltventilTaster].ValueX10 != WWVentilValue) {
      Values[_ThermeUmschaltventilTaster].ValueX10 = WWVentilValue;
      Values[_ThermeUmschaltventilTaster].Changed = 1;
      if (wwLogLevel > 1) {
        Debug.println ( F("ThermeUmschaltventilTaster = "));
        Debug.println ( WWVentilValue);
        Debug.println ( F( "Zugewiesen"));
      }
    }
    if (WWVentilValue != WWVentilStellungWarmwasser) { // Wenn das HappyTemp-Relay nicht geschalten ist oder der Taster nicht Stellung Warmwasser erkennt
      if (WWControlState != 2) {
        WWControlState = 1;                              // und Status auf 1 zurücksetzen
      }
    } 
    else {
      switch (WWControlState) {                        // Ventil steht richtig
      case 4:
        if ( (uint16_t)millis() - lastThermeVorlaufTempVorgabeCheck > 60000) { // Nach 60 Sek mal neu rechnen auch wenn sich keine Temperaturen geändert haben
          ThermeVorlaufTempVorgabeRechnen();
        }
        break;
      case 3:                                          // ThermeVorlaufTempSollX10 durch anpassung der PWM gemäß ThermeVorlaufTempVorgabe nachführen   
        static uint8_t  ThermeVorlaufTempVorgabeValue = 0;       
        ThermeVorlaufTempVorgabeValue = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe * 10 - Values[_ThermeVorlaufTempSoll].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
        analogWrite( ThermeVorlaufTempVorgabePin, ThermeVorlaufTempVorgabeValue);                                                        // PWM setzen
        if (wwLogLevel > 1) {
          Debug.print ("WWControlState=3, Setze ThermeVorlaufTempVorgabeValue auf ");
          Debug.println (ThermeVorlaufTempVorgabeValue);
        }
        lastThermeVorlaufTempVorgabeCheck = (uint16_t)millis();                                                                                 // Wartezeit initialisieren
        WWControlState = 4;                                                                                                        // und in Status4 wechseln
        break;
      case 1:                                                      // Der Taster könnte aberschließen bevor der Motor steht
        digitalWrite(WWVentilSperrenPin, LOW); // den Schrittmotor freigeben
        WWVentilOKSince = (uint16_t)millis();                      // deswegen beginnen wir eine Wartezeit
        if (wwLogLevel > 1) {
          Debug.println( F( "WWVentilSperrenPin,LOW"));
          Debug.print  ( (uint16_t)millis());
          Debug.print  ( F( ": WWVentilOKSince=")); 
          Debug.println( WWVentilOKSince);
        }
        WWControlState = 2;
        break;
      case 2:                                            
        if ((uint16_t)millis() - WWVentilOKSince > 10000){            // Wartezeit ist abgelaufen
          digitalWrite( WWVentilSperrenPin,HIGH);       // das Ventil sperren
          if (wwLogLevel > 1) {Debug.print((uint16_t)millis());Debug.println( F(": WWVentilSperrenPin,HIGH"));}
          WWControlState = 3;
        }
      }
    }
  }
}





