#define Debug Serial
#define owLogLevel 1
typedef struct {
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
} data;

#define _Vorlauf          0
#define _Ruecklauf        1
#devine _Liter            2
#define _WattCounterMinus 3
#define _WattCounterPlus  4

#define ValueUnknown -10000
data Values[] = {
  { 0, ValueUnknown    }, 
  { 0, ValueUnknown    },  
  { 0, 0               },  
  { 0, 0               },  
  { 0, 0               }
}; 
void setup() {
  Serial.begin(115200);
  pinMode(9     , OUTPUT);
  pinMode(13    , OUTPUT);
}
void loop() {
  static uint32_t t2 = 0;
  if ( millis() - t2 > 10000) {
    t2=millis();
    digitalWrite (9, HIGH);
    digitalWrite (9, LOW);
    Loging();
  }
}

boolean setValue( uint8_t valueID, int16_t value) {
  if (Values[valueID].ValueX10 != value) {
    Values[valueID].ValueX10 = value;
    Values[valueID].Changed  = 1;
    return true;
  }  
  else {
    return false;
  }
}

void Loging() {
  static uint32_t t = 0;
  if ( millis() - t > 2000) {
    t=millis();
    Debug.print ( F(" Vorlauf = "));
    Debug.print ( Values[_Vorlauf].ValueX10);
    Debug.print ( F(", Ruecklauf = "));
    Debug.print ( Values[_Ruecklauf].ValueX10);
    Debug.print ( F(", Liter = "));
    Debug.print ( Values[_Liter].ValueX10 );
    Debug.print ( F(", WattCounterMinus = "));
    Debug.print ( Values[_WattCounterMinus].ValueX10);
    Debug.print ( F(", WattCounterPlus = "));
    Debug.print ( Values[_WattCounterPlus].ValueX10);
  }
}
