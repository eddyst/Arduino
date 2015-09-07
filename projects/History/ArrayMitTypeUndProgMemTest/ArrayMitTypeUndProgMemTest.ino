typedef struct {

  uint8_t Changed  ;
  int16_t ValueX10 ;

/*
  uint8_t Changed  :   1;
  int16_t ValueX10 :  15;
*/
  PROGMEM const char  *Name;
} 
Data;
prog_char ThermeKesselTempSoll_Name[] PROGMEM =  "ThermeKesselTempSoll";
prog_char ThermeKesselTempIst_Name [] PROGMEM =  "ThermeKesselTempIst";
#define ValueUnknown -10000
Data Values[2]={
  { 
    0, ValueUnknown, ThermeKesselTempSoll_Name  }
  ,
  { 
    0, ValueUnknown, ThermeKesselTempIst_Name  }
};


void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  Values[0].Changed  =   1;
  Values[0].ValueX10 = 111;
  Values[1].ValueX10 = 222;
  char buffer[30];
  
  strcpy_P( buffer, (char*)pgm_read_word(&(Values[0].Name)));
  Serial.println( buffer);
}

void loop() {
  // put your main code here, to run repeatedly: 

}


