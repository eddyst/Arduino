#define OneWireBus1Pin 11
#define dsDebug

#include <OneWire.h>
OneWire  ds(OneWireBus1Pin); 

void oneWireInit() {
}

void oneWireDoEvents() {
  static uint8_t dsState = 1;
  static byte dsAddr[8];
  static uint16_t waitSince; 
#define dsStateSEARCH             0
#define dsStateWAIT_BEFORE_RESET  1
#define dsStateWAIT_AFTER_RESET   2
#define dsStateSELECT             3
#define dsStateREAD               4
  switch (dsState) {
  case dsStateSEARCH:
    if ( !ds.search( dsAddr)) {
      dsStateChange(&dsState, dsStateWAIT_BEFORE_RESET);
      waitSince = (uint16_t)millis();
    } 
    else {
      dsStateChange(&dsState, dsStateSELECT);
    }
    break;
  case dsStateWAIT_BEFORE_RESET:
    if ((uint16_t)millis() - waitSince >= 20000) {
      dsDebugPrint(F("ds: reset_search"));
      ds.reset_search();

      dsStateChange(&dsState, dsStateWAIT_AFTER_RESET);
      waitSince = (uint16_t)millis();
    }
    break;
  case dsStateWAIT_AFTER_RESET:
    if ((uint16_t)millis() - waitSince >= 250) {
      dsStateChange(&dsState, dsStateSELECT);
    }
    break;
  case dsStateSELECT:
    if (OneWire::crc8(dsAddr, 7) != dsAddr[7]) {
      Debug.println();
      Debug.print(F("ds: ROM ="));
      for(byte i = 0; i < 8; i++) {
        Debug.print(F(" "));
        Debug.print(dsAddr[i], HEX);
      }
      Debug.println(F(" CRC is not valid!"));
      dsStateChange(&dsState, dsStateSEARCH);
      return;
    }

    ds.reset();
    ds.select(dsAddr);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    waitSince = (uint16_t)millis();
    dsStateChange(&dsState, dsStateREAD);
    break;
  case dsStateREAD:
    if ((uint16_t)millis() - waitSince >= 1000) {  // maybe 750ms is enough, maybe not
      // we might do a ds.depower() here, but the reset will take care of it.
      byte present = 0;
      present = ds.reset();
      ds.select(dsAddr);    
      ds.write(0xBE);         // Read Scratchpad

      // the first ROM byte indicates which chip
      byte type_s;
      switch (dsAddr[0]) {
      case 0x10:
        dsDebugPrint(F("  Chip = DS18S20"));  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        dsDebugPrint(F("  Chip = DS18B20"));
        type_s = 0;
        break;
      case 0x22:
        dsDebugPrint(F("  Chip = DS1822"));
        type_s = 0;
        break;
      default:
        Debug.println(F("Device is not a DS18x20 family device."));
        dsStateChange(&dsState, dsStateSEARCH);
        return;
      } 
      // Read Data
      byte data[12];
      dsDebugPrint(F("  Data = "));
      dsDebugPrint(present,HEX);
      dsDebugPrint(F(" "));
      for (byte i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
        dsDebugPrint(data[i], HEX);
        dsDebugPrint(F(" "));
      }
      dsDebugPrint(F(" CRC="));
      dsDebugPrint(OneWire::crc8(data, 8), HEX);
      dsDebugPrintln("");
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
      Debug.print(F("ds: ROM ="));
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        Debug.print(dsAddr[i], HEX);
      }
      Debug.print(F(" Temperature = "));
      Debug.print(celsius);
      Debug.println(F(" C "));
      dsStateChange(&dsState, dsStateSEARCH);
      for (uint8_t i = 0; i < sizeof(owArray); i++) {
        boolean OK = true;
        dsDebugPrint(F(" i = "));
        dsDebugPrint(i, DEC);        
        for (uint8_t b = 0; b < 8; b++) {
          dsDebugPrint(F("   b  = "));
          dsDebugPrint(b,DEC);
          uint8_t ee = EEPROM.read(i * 8 + b);
          dsDebugPrint(F("   ee( "));
          dsDebugPrint(i * 8 + b,DEC);
          dsDebugPrint(F(" ) = "));
          dsDebugPrint(ee,DEC);
          if (dsAddr[b] != ee) {
            dsDebugPrintln(F(" --- NOT EQUAL --- "));
            OK = false;
            break;
          }
        }
        if (OK) {
          if (Values[owArray[i]].ValueX10 != celsius * 10) {
            Values[owArray[i]].ValueX10 = celsius * 10;
            Values[owArray[i]].Changed = true;
            Debug.print(F(" Zugewiesen an "));           
            Debug.println(owArray[i]); 
          }
          return;
        }
      }  
      Debug.println(F("Addresse des Sensor nicht bekannt")); 
    }
    break;
  }
}

void dsStateChange (uint8_t *oldState, uint8_t newState) {
#ifdef dsDebug
  Debug.print(F("ds: State "));
  dsStatePrintName(*oldState);
  Debug.print(F("->"));
  dsStatePrintName(newState);
  Debug.print(F("\n"));
#endif
  *oldState=newState;
}
void dsStatePrintName (uint8_t State) {
  switch (State) {
  case dsStateSEARCH       : 
    Debug.print(F("SEARCH")); 
    break;
  case dsStateWAIT_BEFORE_RESET  : 
    Debug.print(F("WAIT_BEFORE_RESET")); 
    break;
  case dsStateWAIT_AFTER_RESET : 
    Debug.print(F("WAIT_AFTER_RESET")); 
    break;
  case dsStateSELECT           : 
    Debug.print(F("SELECT")); 
    break;
  case dsStateREAD             : 
    Debug.print(F("READ")); 
    break;
  }

}

void dsDebugPrint (char *Msg) {
#ifdef dsDebug
  Debug.print(Msg);
#endif
}
void dsDebugPrintln (char *Msg) {
#ifdef dsDebug
  Debug.print(Msg);
#endif
}
void dsDebugPrint (const __FlashStringHelper* Msg) {
#ifdef dsDebug
  Debug.print(Msg);
#endif
}
void dsDebugPrintln (const __FlashStringHelper* Msg) {
#ifdef dsDebug
  Debug.print(Msg);
#endif
}
void dsDebugPrint (byte Msg, int Type) {
#ifdef dsDebug
  Debug.print(Msg, Type);
#endif
}








