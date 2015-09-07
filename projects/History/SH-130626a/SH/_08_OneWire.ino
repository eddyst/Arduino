#define OneWireBus1Pin 48
#define OneWireBus2Pin 46
#define owDebug

#include <OneWire.h>
OneWire* owBus = NULL;
OneWire  owBus1(OneWireBus1Pin); 
OneWire  owBus2(OneWireBus2Pin); 

void oneWireInit() {
}

void oneWireDoEvents() {
#define owStateBUS_SELECT         0
#define owStateWAIT_BEFORE_RESET  1
#define owStateRESET_SEARCH       2
#define owStateWAIT_AFTER_RESET   3
#define owStateSEARCH             4
#define owStateSELECT             5
#define owStateREAD               6
  static uint8_t owState = owStateBUS_SELECT;
  static byte owAddr[8];
  static uint32_t waitSince; 
  switch (owState) {
  case owStateBUS_SELECT:
    if (owBus == &owBus1){
      owBus = &owBus2;
      if (owLogLevel > 1) Debug.println(F("ow: Bus2")); 
    } 
    else {
      owBus = &owBus1;
      if (owLogLevel > 1) Debug.println(F(":ow Bus1")); 
      waitSince = millis();
      owStateChange(&owState, owStateWAIT_BEFORE_RESET);
      return;
    }
    owStateChange(&owState, owStateRESET_SEARCH);
    break;
  case owStateWAIT_BEFORE_RESET:
    if (millis() - waitSince >= 20000) {
      owStateChange(&owState, owStateRESET_SEARCH);
    }
    break;
  case owStateRESET_SEARCH:
    owBus->reset_search();
    waitSince = millis();
    owStateChange(&owState, owStateWAIT_AFTER_RESET);
    break;
  case owStateWAIT_AFTER_RESET:
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
      owStateChange(&owState, owStateSELECT);
    }
    break;
  case owStateSELECT:
    if (OneWire::crc8(owAddr, 7) != owAddr[7]) {
      if (owLogLevel > 0) Debug.print(F("\now: ROM ="));
      for(byte i = 0; i < 8; i++) {
        if (owLogLevel > 0) Debug.print(F(" "));
        if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
      }
      if (owLogLevel > 0) Debug.println(F(" Addr-CRC is not valid!"));
      owStateChange(&owState, owStateSEARCH);
      return;
    }

    owBus->reset();
    owBus->select(owAddr);
    owBus->write(0x44,1);         // start conversion, with parasite power on at the end

    waitSince = millis();
    owStateChange(&owState, owStateREAD);
    break;
  case owStateREAD:
    if (millis() - waitSince >= 1000) {  // maybe 750ms is enough, maybe not
      // we might do a ow.depower() here, but the reset will take care of it.
      byte present = 0;
      present = owBus->reset();
      owBus->select(owAddr);    
      owBus->write(0xBE);         // Read Scratchpad

      // the first ROM byte indicates which chip
      byte type_s;
      switch (owAddr[0]) {
      case 0x10:
        if (owLogLevel > 2) Debug.print(F("  Chip = DS18S20"));  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        if (owLogLevel > 2) Debug.print(F("  Chip = DS18B20"));
        type_s = 0;
        break;
      case 0x22:
        if (owLogLevel > 2) Debug.print(F("  Chip = DS1822"));
        type_s = 0;
        break;
      default:
        if (owLogLevel > 0) Debug.println(F("Device is not a DS18x20 family device."));
        owStateChange(&owState, owStateSEARCH);
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
      /*      if (owLogLevel > 1){ 
       Debug.print(F(" CRC="));
       Debug.print(OneWire::crc8(data, 8), HEX);
       Debug.println("");
       }
       */
      if (OneWire::crc8(data, 8) != data[8]) {
        if (owLogLevel > 0) Debug.print(F("\now: ROM ="));
        for(byte i = 0; i < 8; i++) {
          if (owLogLevel > 0) Debug.print(F(" "));
          if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
        }
        if (owLogLevel > 0) Debug.println(F(" Data-CRC is not valid!"));
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
      float celsius = (float)raw / 16.0;
      if (owLogLevel > 1) {
        Debug.print(F("ow: ROM ="));
        for(byte i = 0; i < 8; i++) {
          Debug.write(' ');
          if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
        }
        Debug.print(F(" Temperature = "));
        Debug.print(celsius);
        Debug.print(F(" C "));
      }
      owStateChange(&owState, owStateSEARCH);
      if (celsius < -30 || celsius > 125) {
        if (owLogLevel > 0) {
          if (owLogLevel == 1){ // Ansonsten steht die Addr schon da
            Debug.print(F("ow: ROM ="));
            for(byte i = 0; i < 8; i++) {
              Debug.write(' ');
              if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
            }
          }
          Debug.println(F("\n\n\n  Temperature out of Range \n\n\n "));
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
            if (owAddr[b] != ee) {
              if (owLogLevel > 2) Debug.println(F(" --- NOT EQUAL --- "));
              OK = false;
              break;
            }
          }
          if (OK) {
            if (owLogLevel > 1) {
              Debug.print(F(" = "));           
              Debug.print(Values[owArray[i].ValueIndex].Name); 
            }
            celsius = celsius * 10;
            if ( owArray[i].Last1 == owArray[i].Last2) {
              if (owLogLevel > 1) Debug.println(F(" Vorwert hat sich nicht geändert!"));           
            }             
            else if ( owArray[i].Last2 == celsius) { // Sensorflackern
              if (owLogLevel > 1) Debug.println(F(" Vorwert war Sensorflackern!"));           
            }
            else if ( abs( owArray[i].Last1 - owArray[i].Last2) > 10
              && abs( owArray[i].Last1 - celsius         ) > 10 ){// Ausreißer
              if (owLogLevel > 1) Debug.println(F(" Vorwert war Ausreißer!"));           
            }           
            else {
              Values[owArray[i].ValueIndex].ValueX10 = owArray[i].Last1;
              Values[owArray[i].ValueIndex].Changed = true;
              if (owLogLevel > 1) Debug.println(F(" Vorwert zugewiesen"));           
            }
            owArray[i].Last2 = owArray[i].Last1;
            owArray[i].Last1 = celsius ;
            return;
          }
        }  
        if (owLogLevel > 0) {          
          Debug.print(F("ow: ROM = "));
          for(byte i = 0; i < 8; i++) {
            Debug.write(' ');
            if (owLogLevel > 0) Debug.print(owAddr[i], HEX);
          }
          Debug.println(F("Addresse des Sensor nicht bekannt")); 
        }
      }
    }
    break;
  }
}

void owStateChange (uint8_t *oldState, uint8_t newState) {
  if (owLogLevel > 2){ 
    Debug.print(F("ow: State "));
    owStatePrintName(*oldState);
    Debug.print(F("->"));
    owStatePrintName(newState);
    Debug.println(F(""));
  }
  *oldState=newState;
}
void owStatePrintName (const uint8_t& State) {
  switch (State) {
  case owStateBUS_SELECT:
    Debug.print(F("owStateBUS_SELECT")); 
    break;
  case owStateWAIT_BEFORE_RESET: 
    Debug.print(F("WAIT_BEFORE_RESET")); 
    break;
  case owStateRESET_SEARCH: 
    Debug.print(F("owStateRESET_SEARCH")); 
    break;
  case owStateWAIT_AFTER_RESET: 
    Debug.print(F("WAIT_AFTER_RESET")); 
    break;
  case owStateSEARCH: 
    Debug.print(F("SEARCH")); 
    break;
  case owStateSELECT: 
    Debug.print(F("SELECT")); 
    break;
  case owStateREAD: 
    Debug.print(F("READ")); 
    break;
  default: 
    Debug.print(F("Unknown State '"));
    Debug.print(State);
    Debug.print(F("'"));
  }
}









