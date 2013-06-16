#define OneWireBus1Pin 48
#define OneWireBus2Pin 46
#define dsDebug

#include <OneWire.h>
OneWire* owBus = NULL;
OneWire  owBus1(OneWireBus1Pin); 
OneWire  owBus2(OneWireBus2Pin); 

void oneWireInit() {
}

void oneWireDoEvents() {
#define dsStateBUS_SELECT         0
#define dsStateWAIT_BEFORE_RESET  1
#define dsStateRESET_SEARCH       2
#define dsStateWAIT_AFTER_RESET   3
#define dsStateSEARCH             4
#define dsStateSELECT             5
#define dsStateREAD               6
  static uint8_t dsState = dsStateBUS_SELECT;
  static byte dsAddr[8];
  static uint32_t waitSince; 
  switch (dsState) {
  case dsStateBUS_SELECT:
    if (owBus == &owBus1){
      owBus = &owBus2;
      if (owLogLevel > 1) Debug.println(F("  ow Bus2")); 
    } 
    else {
      owBus = &owBus1;
      if (owLogLevel > 1) Debug.println(F("  ow Bus1")); 
      waitSince = millis();
      dsStateChange(&dsState, dsStateWAIT_BEFORE_RESET);
      return;
    }
    dsStateChange(&dsState, dsStateRESET_SEARCH);
    break;
  case dsStateWAIT_BEFORE_RESET:
    if (millis() - waitSince >= 20000) {
      dsStateChange(&dsState, dsStateRESET_SEARCH);
    }
    break;
  case dsStateRESET_SEARCH:
    owBus->reset_search();
    waitSince = millis();
    dsStateChange(&dsState, dsStateWAIT_AFTER_RESET);
    break;
  case dsStateWAIT_AFTER_RESET:
    if (millis() - waitSince >= 250) {
      dsStateChange(&dsState, dsStateSEARCH);
    }
    break;
  case dsStateSEARCH:
    if ( !owBus->search( dsAddr)) {
      dsStateChange(&dsState, dsStateBUS_SELECT);
      waitSince = millis();
    } 
    else {
      dsStateChange(&dsState, dsStateSELECT);
    }
    break;
  case dsStateSELECT:
    if (OneWire::crc8(dsAddr, 7) != dsAddr[7]) {
      if (owLogLevel > 0) Debug.print(F("\nds: ROM ="));
      for(byte i = 0; i < 8; i++) {
        if (owLogLevel > 0) Debug.print(F(" "));
        if (owLogLevel > 0) Debug.print(dsAddr[i], HEX);
      }
      if (owLogLevel > 0) Debug.println(F(" CRC is not valid!"));
      dsStateChange(&dsState, dsStateSEARCH);
      return;
    }

    owBus->reset();
    owBus->select(dsAddr);
    owBus->write(0x44,1);         // start conversion, with parasite power on at the end

    waitSince = millis();
    dsStateChange(&dsState, dsStateREAD);
    break;
  case dsStateREAD:
    if (millis() - waitSince >= 1000) {  // maybe 750ms is enough, maybe not
      // we might do a ds.depower() here, but the reset will take care of it.
      byte present = 0;
      present = owBus->reset();
      owBus->select(dsAddr);    
      owBus->write(0xBE);         // Read Scratchpad

      // the first ROM byte indicates which chip
      byte type_s;
      switch (dsAddr[0]) {
      case 0x10:
        if (owLogLevel > 1) Debug.print(F("  Chip = DS18S20"));  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        if (owLogLevel > 1) Debug.print(F("  Chip = DS18B20"));
        type_s = 0;
        break;
      case 0x22:
        if (owLogLevel > 1) Debug.print(F("  Chip = DS1822"));
        type_s = 0;
        break;
      default:
        if (owLogLevel > 0) Debug.println(F("Device is not a DS18x20 family device."));
        dsStateChange(&dsState, dsStateSEARCH);
        return;
      } 
      // Read Data
      byte data[12];
      if (owLogLevel > 1) {
        Debug.print(F("  Data = "));
        Debug.print(present,HEX);
        Debug.print(F(" "));
      }
      for (byte i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = owBus->read();
        if (owLogLevel > 1){ 
          Debug.print(data[i], HEX);
          Debug.print(F(" "));
        }
      }
      if (owLogLevel > 1){ 
        Debug.print(F(" CRC="));
        Debug.print(OneWire::crc8(data, 8), HEX);
        Debug.println("");
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
      float celsius = (float)raw / 16.0;
      if (owLogLevel > 0) Debug.print(F("ds: ROM ="));
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        if (owLogLevel > 0) Debug.print(dsAddr[i], HEX);
      }
      if (owLogLevel > 0){ 
        Debug.print(F(" Temperature = "));
        Debug.print(celsius);
        Debug.println(F(" C "));
      }
      dsStateChange(&dsState, dsStateSEARCH);
      if (celsius < -30 || celsius > 125) {
        if (owLogLevel > 0){ 
          Debug.print(F("\n\n\n  Temperature out of Range \n\n\n "));
        }

      } 
      else {
        for (uint8_t i = 0; i < sizeof(owArray); i++) {
          boolean OK = true;
          if (owLogLevel > 2) {
            Debug.print(F(" i = "));
            Debug.print(i, DEC);  
          }      
          for (uint8_t b = 0; b < 8; b++) {
            uint8_t ee = EEPROM.read( EEPROM_Offset_owArray + i * 8 + b);
            if (owLogLevel > 2){ 
              Debug.print(F("   b  = "));
              Debug.print(b,DEC);
              Debug.print(F("   ee( "));
              Debug.print(i * 8 + b,DEC);
              Debug.print(F(" ) = "));
              Debug.print(ee,DEC);
            }
            if (dsAddr[b] != ee) {
              if (owLogLevel > 2) Debug.println(F(" --- NOT EQUAL --- "));
              OK = false;
              break;
            }
          }
          if (OK) {
            if (Values[owArray[i]].ValueX10 != celsius * 10) {
              Values[owArray[i]].ValueX10 = celsius * 10;
              Values[owArray[i]].Changed = true;
              if (owLogLevel > 0) {
                Debug.print(F(" Zugewiesen an "));           
                Debug.println(owArray[i]); 
              }
            }
            return;
          }
        }  
        if (owLogLevel > 0) Debug.println(F("Addresse des Sensor nicht bekannt")); 
      }
    }
    break;
  }
}

void dsStateChange (uint8_t *oldState, uint8_t newState) {
  if (owLogLevel > 1){ 
    Debug.print(F("ds: State "));
    dsStatePrintName(*oldState);
    Debug.print(F("->"));
    dsStatePrintName(newState);
    Debug.println(F(""));
  }
  *oldState=newState;
}
void dsStatePrintName (const uint8_t& State) {
  switch (State) {
  case dsStateBUS_SELECT:
    Debug.print(F("dsStateBUS_SELECT")); 
    break;
  case dsStateWAIT_BEFORE_RESET: 
    Debug.print(F("WAIT_BEFORE_RESET")); 
    break;
  case dsStateRESET_SEARCH: 
    Debug.print(F("dsStateRESET_SEARCH")); 
    break;
  case dsStateWAIT_AFTER_RESET: 
    Debug.print(F("WAIT_AFTER_RESET")); 
    break;
  case dsStateSEARCH: 
    Debug.print(F("SEARCH")); 
    break;
  case dsStateSELECT: 
    Debug.print(F("SELECT")); 
    break;
  case dsStateREAD: 
    Debug.print(F("READ")); 
    break;
  default: 
    Debug.print(F("Unknown State '"));
    Debug.print(State);
    Debug.print(F("'"));
  }
}





