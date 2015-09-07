#define HKPumpeAnPin         31
#define HKMischerBewegenPin  32
#define HKMischerRichtungPin 33

#define KAELTER LOW
#define WAERMER HIGH
#define HKBEWEGEN HIGH
#define HKSTOP    LOW

#define d -500
// the setup routine runs once when you press reset:
void setup() {      
  Serial.begin(57600);  
  pinMode     (HKMischerRichtungPin, OUTPUT   );  // Zum initialisieren Mischer erstmal zu drehen 
  pinMode     (HKMischerBewegenPin , OUTPUT   );
/*
  digitalWrite(HKMischerBewegenPin , HKBEWEGEN);
  digitalWrite(HKMischerRichtungPin, KAELTER  );
  Serial.println( "a");
  delay (10000);
  digitalWrite(HKMischerRichtungPin, WAERMER  );
  Serial.println( "b");
  delay (10000);
  digitalWrite(HKMischerBewegenPin , HKSTOP);
  digitalWrite(HKMischerRichtungPin, KAELTER  );
  Serial.println( "c");
  delay (10000);
  digitalWrite(HKMischerRichtungPin, WAERMER  );
  Serial.println( "d");
  delay (10000);
  */
if ( d < 0 )
  digitalWrite(HKMischerRichtungPin, WAERMER  );
else
  digitalWrite(HKMischerRichtungPin, KAELTER  );
  
  digitalWrite(HKMischerBewegenPin , HKBEWEGEN);
  delay (10000);
  digitalWrite(HKMischerBewegenPin , HKSTOP);
  delay (2000);
}

// the loop routine runs over and over again forever:
void loop() {
  static uint16_t i = 0;
  Serial.println( i++);
  
  if ( d < 0 )
    digitalWrite (HKMischerRichtungPin, KAELTER );
  else
    digitalWrite (HKMischerRichtungPin, WAERMER );
  digitalWrite (HKMischerBewegenPin , HKBEWEGEN);
  delay(abs(d));
  digitalWrite (HKMischerBewegenPin , HKSTOP);
  delay (1000);
  }

