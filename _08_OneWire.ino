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
  switch (dsState) {
  case 0:
    if ((uint16_t)millis() - waitSince >= 10000) {
      dsState = 3;
    }
    break;
  case 1:
    if ( !ds.search( dsAddr)) {
#ifdef dsDebug
      Debug.println("ds: reset_search");
#endif
      ds.reset_search();
      waitSince = (uint16_t)millis();
      dsState = 2;
    } 
    else {
      dsState = 3;
    }
  case 2:
    if ((uint16_t)millis() - waitSince >= 250) {
      dsState = 3;
    }
    break;
  case 3:
      Debug.println();
      Debug.print("ds: ROM =");
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        Debug.print(dsAddr[i], HEX);
      }
    if (OneWire::crc8(dsAddr, 7) != dsAddr[7]) {
      Debug.println();
      Debug.print("ds: ROM =");
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        Debug.print(dsAddr[i], HEX);
      }
      Debug.println(" CRC is not valid!");
      return;
    }

    ds.reset();
    ds.select(dsAddr);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    waitSince = (uint16_t)millis();
    dsState = 4;
    break;
  case 4:
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
#ifdef dsDebug
        Debug.println("  Chip = DS18S20");  // or old DS1820
#endif
        type_s = 1;
        break;
      case 0x28:
#ifdef dsDebug
        Debug.println("  Chip = DS18B20");
#endif        
        type_s = 0;
        break;
      case 0x22:
#ifdef dsDebug
        Debug.println("  Chip = DS1822");
#endif
        type_s = 0;
        break;
      default:
        Debug.println("Device is not a DS18x20 family device.");
        return;
      } 
      // Read Data
      byte data[12];
#ifdef dsDebug
      Debug.print("  Data = ");
      Debug.print(present,HEX);
      Debug.print(" ");
#endif
      for (byte i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
#ifdef dsDebug
        Debug.print(data[i], HEX);
        Debug.print(" ");
#endif
      }
#ifdef dsDebug
      Debug.print(" CRC=");
      Debug.print(OneWire::crc8(data, 8), HEX);
      Debug.println();
#endif
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
      Debug.print("ds: ROM =");
      for(byte i = 0; i < 8; i++) {
        Debug.write(' ');
        Debug.print(dsAddr[i], HEX);
      }
      Debug.print(" Temperature = ");
      Debug.print(celsius);
      Debug.print(" C ");
      dsState = 0;
      for (uint8_t i = 0; i < (sizeof(owAddr) / 9) - 1; i++) {
        boolean OK = true;
        for (uint8_t b = 0; b < 8; i++) {
          if (dsAddr[b] != owAddr [i][b]) {
            OK = false;
            break;
          }
        }
        if (OK) {
          if (Values[i].ValueX10 != celsius * 10) {
            Values[i].ValueX10 = celsius * 10;
            Values[i].Changed = true;
            Debug.print(" Zugewiesen an ");           
            Debug.println(i); 
          }
          return;
        }
      }  
      Debug.println("Addresse des Sensor nicht bekannt"); 
      waitSince = (uint16_t)millis();
    }
    break;
  }
}






