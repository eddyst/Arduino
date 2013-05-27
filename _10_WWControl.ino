#define HappyTempPin 1
#define WWVentilPin 2
#define WWVentilStellungWarmwasser HIGH
#define WWVentilSperrenPin 3
#define ExternAnfordernPin 4
#define ThermeVorlaufTempVorgabePin A1

uint8_t WWControlState = 0;

void WarmwasserBegin(){
  Debug.println ("WarmwasserBegin()");
  digitalWrite(HappyTempPin, HIGH);
  WWControlState=1;
}

void WarmwasserEnde(){
  Debug.println ("WarmwasserEnde()");
  digitalWrite(HappyTempPin, LOW);
  WWControlState=0;
}

void ThermeVorlaufTempVorgabeRechnen() {
  Debug.println ("ThermeVorlaufTempVorgabeRechnen()");
  if (WWControlState == 4) {
    WWControlState = 3;
  }
}
void wwInit() {
  pinMode     (HappyTempPin, OUTPUT);
  digitalWrite(HappyTempPin, LOW);

  pinMode     (WWVentilPin , INPUT_PULLUP); //Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal 
  //20K-ohm resistor is pulled to 5V. This configuration causes the input to 
  //read HIGH when the switch is open, and LOW when it is closed. 
  pinMode     (ThermeVorlaufTempVorgabePin, OUTPUT);
  analogWrite (ThermeVorlaufTempVorgabePin, 0     );
}

void   WWControlDoEvents() {
  static uint16_t lastThermeVorlaufTempVorgabeCheck;
  uint16_t millis16 = millis();
  if (WWControlState >  0) {
    static uint16_t WWVentilOKSince;
    boolean WWVentilValue = digitalRead(WWVentilPin);
    if (Values[_ThermeUmschaltventilTaster].ValueX10 != WWVentilValue) {
      Values[_ThermeUmschaltventilTaster].ValueX10 = WWVentilValue;
      Values[_ThermeUmschaltventilTaster].Changed = 1;
      Debug.println ("ThermeUmschaltventilTaster Zugewiesen");
    }
    if (WWVentilValue != WWVentilStellungWarmwasser) { // Wenn das HappyTemp-Relay nicht geschalten ist oder der Taster nicht Stellung Warmwasser erkennt
      digitalWrite(WWVentilSperrenPin,LOW);            // den Schrittmotor freigeben
      Debug.println ("WWVentilSperrenPin,LOW");
      WWControlState = 1;                              // und Status auf 1 zurücksetzen
    } 
    else {
      switch (WWControlState) {                        // Ventil steht richtig
      case 4:
        if ( millis16 - lastThermeVorlaufTempVorgabeCheck > 60000) { // Nach 60 Sek mal neu rechnen auch wenn sich keine Temperaturen geändert haben
          ThermeVorlaufTempVorgabeRechnen();
        }
        break;
      case 3:                                          // ThermeVorlaufTempSollX10 durch anpassung der PWM gemäß ThermeVorlaufTempVorgabe nachführen   
        Debug.print ("WWControlState=3, Setze ThermeVorlaufTempVorgabeValue auf ");
        static uint8_t  ThermeVorlaufTempVorgabeValue = 0;       
        ThermeVorlaufTempVorgabeValue = ThermeVorlaufTempVorgabeValue + (ThermeVorlaufTempVorgabe * 10 - Values[_ThermeVorlaufTempSoll].ValueX10) * 255 /1000; // ThermeVorlaufTempVorgabeValue berechnen
        Debug.println (ThermeVorlaufTempVorgabeValue);
        analogWrite( ThermeVorlaufTempVorgabePin, ThermeVorlaufTempVorgabeValue);                                                        // PWM setzen
        lastThermeVorlaufTempVorgabeCheck = millis16;                                                                                 // Wartezeit initialisieren
        WWControlState = 4;                                                                                                        // und in Status4 wechseln
        break;
      case 1:                                          // Der Taster könnte aberschließen bevor der Motor steht
        WWVentilOKSince = millis16;                      // deswegen beginnen wir eine Wartezeit
        Debug.println ("WWVentilOKSince=millis16");
        WWControlState = 2;
        break;
      case 2:                                            
        if (millis16 - WWVentilOKSince > 3000){            // Wartezeit ist abgelaufen
          digitalWrite( WWVentilSperrenPin,HIGH);       // das Ventil sperren
          Debug.println ( "WWVentilSperrenPin,HIGH");
          WWControlState = 3;
        }
      }
    }
  }
}



