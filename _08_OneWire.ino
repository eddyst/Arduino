#define OneWireBus1Pin 48
#define OneWireBus2Pin 46
//#define UpdateEachSensor
#define parasite 1
#include <OneWire.h>
#include <EEPROM.h>

OneWire* owBus = NULL;
OneWire  owBus1(OneWireBus1Pin); 
OneWire  owBus2(OneWireBus2Pin); 

void doEventsOWMaster() {
#define owStateBUS_SELECT             0
#define owStateDELAY                  1
#define owStateUPDATE                 2
#define owStateWAIT_AFTER_UPDATE      3
#define owStateRESETSEARCH            4
#define owStateWAIT_AFTER_RESETSEARCH 5
#define owStateSEARCH                 6
#define owStateREAD                   7
  static uint8_t owState = owStateBUS_SELECT;
  static byte owAddr[8];
  static uint32_t waitSince; 
  switch (owState) {
  case owStateBUS_SELECT:
    if (owBus == &owBus1){
      owBus = &owBus2;
      if (owLogLevel > 1) Debug.println( F("ow: Bus2")); 
    } 
    else {
      owBus = &owBus1;
      if (owLogLevel > 1) Debug.println( F(":ow Bus1")); 
      waitSince = millis();
      owStateChange(&owState, owStateDELAY);
      return;
    }
#ifdef UpdateEachSensor
    owStateChange(&owState, owStateRESETSEARCH);
#else
    owStateChange(&owState, owStateUPDATE);
#endif    
    break;
  case owStateDELAY:
    if (millis() - waitSince >= 10000) {//20000
#ifdef UpdateEachSensor
      owStateChange(&owState, owStateRESETSEARCH);
#else
      owStateChange(&owState, owStateUPDATE);
#endif    
    }
    break;
  case owStateUPDATE:
    owBus->reset();
#ifdef UpdateEachSensor
    owBus->select(owAddr);
#else
    owBus->write(0xCC);  
#endif
    owBus->write(0x44, parasite);         // start conversion, with parasite power on at the end

    waitSince = millis();
    owStateChange(&owState, owStateWAIT_AFTER_UPDATE);
    break;
  case owStateWAIT_AFTER_UPDATE:
    if (millis() - waitSince >= 1000) {  // maybe 750ms is enough, maybe not
#ifdef UpdateEachSensor
      owStateChange(&owState, owStateREAD);
#else
      owStateChange(&owState, owStateRESETSEARCH);
#endif
    }
    break;
  case owStateRESETSEARCH:
    owBus->reset_search();
    waitSince = millis();
    owStateChange(&owState, owStateWAIT_AFTER_RESETSEARCH);
    break;
  case owStateWAIT_AFTER_RESETSEARCH:
    if (millis() - waitSince >= 250) {
      owStateChange(&owState, owStateSEARCH);
    }
    break;
  case owStateSEARCH:
    if ( !owBus->search( owAddr)) {
      owStateChange(&owState, owStateBUS_SELECT);
      waitSince = millis();
    } 
    else {
      if (OneWire::crc8(owAddr, 7) == owAddr[7]) {
#ifdef UpdateEachSensor
        owStateChange(&owState, owStateUPDATE);
#else
        owStateChange(&owState, owStateREAD);
#endif        
      }
      else {
        if (owLogLevel > 0) Debug.print( F("\now: ROM ="));
        for(byte i = 0; i < 8; i++) {
          if (owLogLevel > 0) Debug.print( F(" "));
          if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
        }
        if (owLogLevel > 0) Debug.println( F(" Addr-CRC is not valid!"));
        owStateChange(&owState, owStateSEARCH);
      }
    }
    break;
  case owStateREAD:
    byte present = 0;
    present = owBus->reset();
    owBus->select(owAddr);    
    owBus->write(0xBE);         // Read Scratchpad

    // the first ROM byte indicates which chip
    byte type_s;
    switch (owAddr[0]) {
    case 0x10:
      if (owLogLevel > 2) Debug.print( F("  Chip = DS18S20"));  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      if (owLogLevel > 2) Debug.print( F("  Chip = DS18B20"));
      type_s = 0;
      break;
    case 0x22:
      if (owLogLevel > 2) Debug.print( F("  Chip = DS1822"));
      type_s = 0;
      break;
      /*      case 0x1D: //DS2423
       if (owLogLevel > 2) Debug.print( F("  Chip = DS2423 Not Handled"));
       break;
       */
    default:
      if (owLogLevel > 0) Debug.println( F("Device is not a DS18x20 family device."));
      owStateChange(&owState, owStateSEARCH);
      return;
    } 
    // Read Data
    byte data[12];
    if (owLogLevel > 1) {
      Debug.print( F("  Data = "));
      Debug.print(present,HEX);
      Debug.print( F(" "));
    }
    for (byte i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = owBus->read();
      if (owLogLevel > 1){ 
        Debug.print(data[i], HEX);
        Debug.print( F(" "));
      }
    }
    /*      if (owLogLevel > 1){ 
     Debug.print( F(" CRC="));
     Debug.print(OneWire::crc8(data, 8), HEX);
     Debug.println("");
     }
     */
    if (OneWire::crc8(data, 8) != data[8]) {
      if (owLogLevel > 0) Debug.print( F("\now: ROM ="));
      for(byte i = 0; i < 8; i++) {
        if (owLogLevel > 0) Debug.print( F(" "));
        if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
      }
      if (owLogLevel > 0) Debug.println( F(" Data-CRC is not valid!"));
      owStateChange(&owState, owStateSEARCH);
      return;
    }
    // convert the data to actual temperature
    unsigned int raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // count remain gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } 
    else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
    float celsius = ( float)raw / 16.0;
    if (owLogLevel > 1) {
      Debug.print( F("ow: ROM ="));
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
      }
      Debug.print( F(" Temperature = "));
      Debug.print(celsius);
      Debug.print( F(" C "));
      if (owLogLevel < 3) Debug.println();
    }
    owStateChange(&owState, owStateSEARCH);
    if (celsius < -30 || celsius > 125) {
      if (owLogLevel > 0) {
        if (owLogLevel == 1){ // Ansonsten steht die Addr schon da
          Debug.print( F("ow: ROM ="));
          for(byte i = 0; i < 8; i++) {
            Debug.write(' ');
            if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
          }
        }
        Debug.println( F("\n\n\n  Temperature out of Range \n\n\n "));
      }
    } 
    else {
      for (tmpUint8_1 = 0; tmpUint8_1 < sizeof(owArray); tmpUint8_1++) {
        boolean OK = true;
        if (owLogLevel > 3) {
          Debug.print( F(" tmpUint8_1 = "));
          Debug.print(tmpUint8_1, DEC);  
        }      
        for (tmpUint8_2 = 0; tmpUint8_2 < 8; tmpUint8_2++) {
          tmpUint8_3 = EEPROM.read( EEPROM_Offset_owArray + tmpUint8_1 * 8 + tmpUint8_2);
          if (owLogLevel > 3){ 
            Debug.print( F("   b  = "));
            Debug.print( tmpUint8_2,DEC);
            Debug.print( F("   ee( "));
            Debug.print( tmpUint8_1 * 8 + tmpUint8_2,DEC);
            Debug.print( F(" ) = "));
            Debug.print( tmpUint8_3, DEC);
          }
          if (owAddr[tmpUint8_2] != tmpUint8_3) {
            if (owLogLevel > 3) Debug.println( F(" --- NOT EQUAL --- "));
            OK = false;
            break;
          }
        }
        if (OK) {
          if ( setValue( owArray[tmpUint8_1], celsius * 10) && owLogLevel > 1) {
            Debug.print( F(" Zugewiesen an "));           
            Debug.println(owArray[tmpUint8_1]); 
          }
          return;
        }
      }  
      if (owLogLevel > 0) {          
        Debug.print( F("ow: ROM = "));
        for(byte i = 0; i < 8; i++) {
          Debug.write(' ');
          if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
        }
        Debug.println( F("Addresse des Sensor nicht bekannt")); 
      }
    }
    break;
  }
}

void owStateChange (uint8_t *oldState, uint8_t newState) {
  if (owLogLevel > 2){ 
    Debug.print( F("ow: State "));
    owStatePrintName(*oldState);
    Debug.print( F("->"));
    owStatePrintName(newState);
    Debug.println( F(""));
  }
  *oldState=newState;
}
void owStatePrintName (const uint8_t& State) {
  switch (State) {
  case owStateBUS_SELECT:
    Debug.print( F("owStateBUS_SELECT")); 
    break;
  case owStateDELAY: 
    Debug.print( F("DELAY")); 
    break;
  case owStateUPDATE: 
    Debug.print( F("UPDATE")); 
    break;
  case owStateWAIT_AFTER_UPDATE: 
    Debug.print( F("WAIT_AFTER_UPDATE")); 
    break;
  case owStateRESETSEARCH: 
    Debug.print( F("owStateRESETSEARCH")); 
    break;
  case owStateWAIT_AFTER_RESETSEARCH: 
    Debug.print( F("WAIT_AFTER_RESETSEARCH")); 
    break;
  case owStateSEARCH: 
    Debug.print( F("SEARCH")); 
    break;
  case owStateREAD: 
    Debug.print( F("READ")); 
    break;
  default: 
    Debug.print( F("Unknown State '"));
    Debug.print(State);
    Debug.print( F("'"));
  }
}









