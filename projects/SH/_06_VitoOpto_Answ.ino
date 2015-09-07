void VitoDebugMsg(uint8_t LogLevel, const char* Msg){
  if (vitoLogLevel > 0) {
    if (vitoLogLevel >= LogLevel) {
//      Debug.print ( F( "\nVito ");
//      Debug.print (vitoLogLevel);
//      Debug.print ( F( "-");
//      Debug.print (LogLevel);
//      Debug.print ( F( ":"); 
      Debug.print (Msg);
    }
  }
}

int16_t ValueInt16;
void VitoOnValueRead(uint8_t AdrBit1, uint8_t AdrBit2, uint8_t Length, uint8_t* Data){
  switch ((AdrBit1 << 8) + AdrBit2) {
    case  (0x55 << 8) + 0x5A: // ThermeKesselTempSoll (/ 10) (2 Byte)
    if (vitoLogLevel > 1) {
      Debug.print( F( "ThermeKesselTempSoll = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[1] * 256 + (uint16_t)Data[0];
    if (vitoLogLevel > 1) {
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    ValueInt16 = (Data[1] << 8) + Data[0];
    if (vitoLogLevel > 1) {
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeKesselTempSoll].ValueX10 != ValueInt16) {
      Values[_ThermeKesselTempSoll].ValueX10 = ValueInt16;
      Values[_ThermeKesselTempSoll].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0xA3 << 8) + 0x93: // KesselTemperatur (/ 100) (2 Byte)
    if (vitoLogLevel > 1) {
      Debug.print( F( "ThermeKesselTempIst = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = ((uint16_t)Data[1] * 256 + (uint16_t)Data[0]) / 10;
    if (vitoLogLevel > 1) {
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeKesselTempIst].ValueX10 != ValueInt16) {
      Values[_ThermeKesselTempIst].ValueX10 = ValueInt16;
      Values[_ThermeKesselTempIst].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x25 << 8) + 0x44: // ThermeVorlaufTempSoll A1M1 (/ 10) (2 Byte)
    if (vitoLogLevel > 1) {
      Debug.print( F( "ThermeVorlaufTempSoll = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[1] * 256 + (uint16_t)Data[0];
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeVorlaufTempSoll].ValueX10 != ValueInt16) {
      Values[_ThermeVorlaufTempSoll].ValueX10 = ValueInt16;
      Values[_ThermeVorlaufTempSoll].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x08 << 8) + 0x08: // ThermeRuecklaufTemp (17A) RLTS (/ 10) (2 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( "ThermeRuecklaufTempOpto = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[1] * 256 + (uint16_t)Data[0];
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeRuecklaufTempOpto].ValueX10 != ValueInt16) {
      Values[_ThermeRuecklaufTempOpto].ValueX10 = ValueInt16;
      Values[_ThermeRuecklaufTempOpto].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x08 << 8) + 0x12: // SpeicherTemperatur Tiefpass STS1 (/ 10) (2 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( "ThermeSpeicherTemp = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[1] * 256 + (uint16_t)Data[0];
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeSpeicherTemp].ValueX10 != ValueInt16) {
      Values[_ThermeSpeicherTemp].ValueX10 = ValueInt16;
      Values[_ThermeSpeicherTemp].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x55 << 8) + 0x25: // Aussentemperatur Tiefpass (/ 10) (2 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( "ThermeAussenTemp = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[1] * 256 + (uint16_t)Data[0];
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeAussenTemp].ValueX10 != ValueInt16) {
      Values[_ThermeAussenTemp].ValueX10 = ValueInt16;
      Values[_ThermeAussenTemp].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x23 << 8) + 0x23: // ThermeBetriebsart A1M1 (1 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( "ThermeBetriebsart = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
    }
    ValueInt16 = (uint16_t)Data[0] * 10;
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.println(ValueInt16);
    }
    if (Values[_ThermeBetriebsart].ValueX10 != ValueInt16) {
      Values[_ThermeBetriebsart].ValueX10 = ValueInt16;
      Values[_ThermeBetriebsart].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0x0A << 8) + 0x10: // Umschaltventil (0..3) (1 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( " ThermeUmschaltventilOpto = ")); 
      Debug.print(Data[0],HEX);
    }
    ValueInt16 = (uint16_t)Data[0] * 10;
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> ")); 
      Debug.print(ValueInt16);
    }
    if (Values[_ThermeUmschaltventilOpto].ValueX10 != ValueInt16) {
      Values[_ThermeUmschaltventilOpto].ValueX10 = ValueInt16;
      Values[_ThermeUmschaltventilOpto].Changed = 1;
      if (vitoLogLevel > 1) Debug.print( F( " Zugewiesen ")); 
    }
    break;
    case  (0xA3 << 8) + 0x8F: // Anlagen Ist-Leistung (0..200 / 0..1) (2 Byte)
    if (vitoLogLevel > 1) { 
      Debug.print( F( "ThermeBrennerLeistung = ")); 
      Debug.print(Data[0],HEX); 
      Debug.print( F( " ")); 
      Debug.print(Data[1],HEX);
    }
    ValueInt16 = (uint16_t)Data[0] * 5;
    if (vitoLogLevel > 1) { 
      Debug.print( F( " --> Leistung% ")); 
      Debug.print( ValueInt16);
    }
    if( Values[_ThermeBrennerLeistung].ValueX10 > 0 && ValueInt16 == 0) 
      ThermeBrennerGehtAus();
    if ( setValue(_ThermeBrennerLeistung, ValueInt16) && vitoLogLevel > 1) {
      Debug.print( F( " Zugewiesen ")); 
    }    break;
    /*      
     case : // Brennerstarts
     Vito.beginReadValue(0x08, 0x8A, 0x04); 
     break; 
     case : // Betriebsstunden Stufe 1 (Sek)
     Vito.beginReadValue(0x08, 0x86, 0x04); 
     break; 
     case : // Sammelstörung (0..1)
     Vito.beginReadValue(0x0A, 0x82, 0x01); 
     break; 
     case : // Sparbetrieb A1M1 (0..1)
     Vito.beginReadValue(0x23, 0x02, 0x01); 
     break; 
     case : // Partybetrieb A1M1 (0..1)
     Vito.beginReadValue(0x23, 0x03, 0x01); 
     break; 
     case : // Speicherladepumpe (0..1)
     Vito.beginReadValue(0x65, 0x13, 0x01); 
     break; 
     case : // Interne Pumpe (0..1 / 0..100)
     Vito.beginReadValue(0x76, 0x60, 0x02); 
     break; 
     case : // Heizkreispumpe A1M1 (0..1 / 0..100)
     Vito.beginReadValue(0x76, 0x63, 0x02); 
     break; 
     */
  default: 
    if (vitoLogLevel > 0) { 
      Debug.println();
      Debug.print( F( "Unbekanntes Result AdrBit1="));
      Debug.print(AdrBit1);
      Debug.print( F( "  AdrBit2="));
      Debug.print(AdrBit2);
    }
  }
  if (vitoLogLevel > 1) Debug.println();
}










