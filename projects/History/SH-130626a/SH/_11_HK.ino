#define HKMischerRichtungPin 33
#define HKMischerBewegenPin  32

#define KAELTER LOW
#define WAERMER HIGH
#define HKBEWEGEN LOW
#define HKSTOP    HIGH
void HKInit(){
  pinMode     (HKMischerRichtungPin, OUTPUT );  // Zum initialisieren Mischer erstmal zu drehen 
  digitalWrite(HKMischerRichtungPin, KAELTER);
  pinMode     (HKMischerBewegenPin , OUTPUT );
  digitalWrite(HKMischerBewegenPin , HKBEWEGEN   );
}

void   HKDoEvents() {
  //HKMischerRichtungPin  HKMischerBewegenPin  HKSollTempVorgabe
  if (Values[_HKVorlaufTemp2].ValueX10 != ValueUnknown) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
    if (Values[_HKVorlaufTemp2].ValueX10 > 600) {  // Zudrehen!!!!
      if (hkLogLevel > 0) Debug.println ("HK: ! >600 = soKALTwiesGEHT");
      digitalWrite (HKMischerRichtungPin, KAELTER );
      digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
    }     
    else {
      static uint32_t lastMove;
      if (millis() - lastMove > 60000) {                         // Wenn min ? Sekunden seit letzter Bewegung
        //Wir haben keine genaue Info zur Ventilpossition deswegen zählen wir hilfsweise bei jeder Bewegung die Schritte in eine Richtung
        if (Values[_HKVorlaufTemp2].ValueX10 > (HKSollTempVorgabe + 5)*10 ) {         // Bei 5 C° Überschreitung
          if (hkLogLevel > 0) Debug.println (F("HK: kälter"));
          if (Values[_HKVorlaufValue].ValueX10 < -20) { 
            //ToDo: Ende von Wärme anfordern

          } 
          else{
            digitalWrite (HKMischerRichtungPin, KAELTER );             // Zudrehen - aber nur ein bissel
            digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
            delay(500);
            digitalWrite (HKMischerBewegenPin , HKSTOP);
            if (Values[_HKVorlaufValue].ValueX10 >= 0) 
              Values[_HKVorlaufValue].ValueX10 = -1;
            else
              Values[_HKVorlaufValue].ValueX10 --;
            Values[_HKVorlaufValue].Changed = 1;
            if (hkLogLevel > 0) {Debug.print (F("    HKVorlaufValue = "));Debug.println(Values[_HKVorlaufValue].ValueX10);}
          }
          lastMove=millis();  
        } 
        else if (Values[_HKVorlaufTemp2].ValueX10 < (HKSollTempVorgabe - 2)*10 ){
          if (hkLogLevel > 0) Debug.println (F("HK: wärmer"));
          if (Values[_HKVorlaufValue].ValueX10 > 20) { 
            //ToDo: Wärme anfordern

          } 
          else{
            digitalWrite (HKMischerRichtungPin, WAERMER );
            digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
            delay(500);
            digitalWrite (HKMischerBewegenPin , HKSTOP);  
            if (Values[_HKVorlaufValue].ValueX10 <= 0) 
              Values[_HKVorlaufValue].ValueX10 = 1;
            else
              Values[_HKVorlaufValue].ValueX10 ++;
            Values[_HKVorlaufValue].Changed = 1;
            if (hkLogLevel > 0) {Debug.print (F("    HKVorlaufValue = "));Debug.println(Values[_HKVorlaufValue].ValueX10);}
          }
          lastMove=millis();  
        }
      }
    }
  }
}








