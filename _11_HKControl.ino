#define HKMischerRichtungPin 5
#define HKMischerBewegenPin 6

void hkInit(){
  pinMode     (HKMischerRichtungPin, OUTPUT);  // Zum initialisieren Mischer erstmal zu drehen 
  digitalWrite(HKMischerRichtungPin, LOW   );
  pinMode     (HKMischerBewegenPin , OUTPUT);
  digitalWrite(HKMischerBewegenPin , HIGH  );
}

void   HKControlDoEvents() {
  //HKMischerRichtungPin  HKMischerBewegenPin  HKSollTempVorgabe
  if (Values[_HKVorlaufGemischt].ValueX10 != ValueUnknown) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
    if (Values[_HKVorlaufGemischt].ValueX10 > 600) {  // Zudrehen!!!!
      if (hkLogLevel > 0) Debug.println ("HK: ! >600 = soKALTwiesGEHT");
      digitalWrite (HKMischerRichtungPin, LOW );
      digitalWrite (HKMischerBewegenPin , HIGH);
    }     
    else {
      static uint32_t lastMove;
      if (millis() - lastMove > 20000) {                         // Wenn min ? Sekunden seit letzter Bewegung
        //Wir haben keine genaue Info zur Ventilpossition deswegen zählen wir hilfsweise bei jeder Bewegung die Schritte in eine Richtung
        if (Values[_HKVorlaufGemischt].ValueX10 > HKSollTempVorgabe + 5 ) {         // Bei 5 C° Überschreitung
          if (hkLogLevel > 0) Debug.println (F("HK: kälter"));
          if (Values[_HKVorlaufValue].ValueX10 < -20) { 
            //ToDo: Ende von Wärme anfordern

          } 
          else{
            digitalWrite (HKMischerRichtungPin, LOW );             // Zudrehen - aber nur ein bissel
            digitalWrite (HKMischerBewegenPin , HIGH);
            delay(500);
            digitalWrite (HKMischerBewegenPin , LOW);
            if (Values[_HKVorlaufValue].ValueX10 >  0) 
              Values[_HKVorlaufValue].ValueX10 = -1;
            else
              Values[_HKVorlaufValue].ValueX10 --;
            Values[_HKVorlaufValue].Changed = 1;
            lastMove=millis();  
          }
        } 
        else if (Values[_HKVorlaufGemischt].ValueX10 < HKSollTempVorgabe - 2 ){
          if (hkLogLevel > 0) Debug.println (F("HK: wärmer"));
          if (Values[_HKVorlaufValue].ValueX10 > 20) { 
            //ToDo: Wärme anfordern

          } 
          else{
            digitalWrite (HKMischerRichtungPin, HIGH );
            digitalWrite (HKMischerBewegenPin , HIGH);
            delay(500);
            digitalWrite (HKMischerBewegenPin , LOW);  
            if (Values[_HKVorlaufValue].ValueX10 <  0) 
              Values[_HKVorlaufValue].ValueX10 = 1;
            else
              Values[_HKVorlaufValue].ValueX10 ++;
            Values[_HKVorlaufValue].Changed = 1;
            lastMove=millis();  
          }
        }
      }
    }
  }
}








