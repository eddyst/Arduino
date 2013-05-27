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
  if (_HKVorlaufGemischt != -128) { //Wenn die Vorlauftemperatur nicht gesetzt wurde können wir nichts regeln
    if (_HKVorlaufGemischt > 60) {  // Zudrehen!!!!
              Debug.println ("HK: ! >60 = soKALTwiesGEHT");
      digitalWrite (HKMischerRichtungPin, LOW );
      digitalWrite (HKMischerBewegenPin , HIGH);
    }     
    else {
      static uint32_t lastMove;
      if (millis() - lastMove > 20000) {                         // Wenn min ? Sekunden seit letzter Bewegung
        if (_HKVorlaufGemischt > HKSollTempVorgabe + 5 ) {         // Bei 5 C° Überschreitung
          Debug.println ("HK: kälter");
          digitalWrite (HKMischerRichtungPin, LOW );             // Zudrehen - aber nur ein bissel
          digitalWrite (HKMischerBewegenPin , HIGH);
          delay(500);
          digitalWrite (HKMischerBewegenPin , LOW);
          lastMove=millis();  
        } 
        else if (_HKVorlaufGemischt < HKSollTempVorgabe - 2 ){
          Debug.println ("HK: wärmer");
          digitalWrite (HKMischerRichtungPin, HIGH );
          digitalWrite (HKMischerBewegenPin , HIGH);
          delay(500);
          digitalWrite (HKMischerBewegenPin , LOW);  
          lastMove=millis();  
        }
      }
    }
  }
}





