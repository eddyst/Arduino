byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // MAC address.
#define owAddrSyncIntervall 3600000
#define SollSyncIntervall     60000

#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;

void EthInit() {
  if (ethLogLevel > 0) Debug.println( F( "eth: Initialising the Ethernet controller"));
  if (Ethernet.begin(mac) == 0) {
    if (ethLogLevel > 0) Debug.println( F( "eth: Failed to configure Ethernet using DHCP"));
  }
  delay(1000);  // give the Ethernet shield a second to initialize:
  if (ethLogLevel > 0) {
    Debug.print( F( "eth: My IP address: "));
    for (byte i = 0; i < 4; i++) {
      Debug.print(Ethernet.localIP()[i], DEC); // print the value of each byte of the IP address:
      Debug.print( F ( ".")); 
    }
    Debug.println();
  }
}

uint8_t digit;
uint8_t  HexCharIndex, AddrByteIndex, Addr[7];
int16_t Value;

boolean First;
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  First = true;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();   
    if ( First){    
      if (ethLogLevel > 1) Debug.print( F("\neth: IN: ")); 
      First=false;
    }
    if (ethLogLevel > 1) Debug.print(c);              // send it out the Debug port. For debugging purposes
    static char ethInBuffer[30];
    static uint8_t recording = 0, ethInBufferCount = 0;
    if (c == '<'){
      if (recording == 0) ethInBufferCount = 0;//Es beginnt eine neue Interessante Zeichenfolge
      recording ++; 
    } 
    else {
      if (recording > 0) {//Wenn kein Empfang läuft brauchen können wir uns den Rest sparen
        if (c == '>') {
          recording --;
          if (recording == 0) {
            if (ethLogLevel > 2) Debug.print( F( "\neth: Zeichenfolge auswerten: ")); 
            for (int i = 0; i < ethInBufferCount; i++) {
              if (ethLogLevel > 2) Debug.write(ethInBuffer[i]);
            }
            if (ethInBufferCount > 7 
              && ethInBuffer[0] == 'S'
              && ethInBuffer[1] == 'o'
              && ethInBuffer[2] == 'l'
              && ethInBuffer[3] == 'l'
              && ethInBuffer[6] == '=' ) {
              if (ethLogLevel > 1) Debug.println(F(" = SollVorgabe"));
              digit;
              tmpUint8_1 = 7;
              Value = 0;
              while(tmpUint8_1 < ethInBufferCount && tmpUint8_1 < 9) {
                if (ethLogLevel > 2) {
                  Debug.print  ( F("  ethInBuffer["));
                  Debug.print  ( tmpUint8_1);
                  Debug.print  ( F("] = ")); 
                  Debug.print  ( ethInBuffer[tmpUint8_1]);
                }
                if (parseHexChar(ethInBuffer[tmpUint8_1], digit)) {//Wir können mit parseHexChar arbeiten weils nur 1 Stelle ist
                  Value = Value * 10 + digit;
                  if (ethLogLevel > 2) {
                    Debug.print  ( F("  Value ="));
                    Debug.println( Value);
                  }
                } 
                else {
                  if (ethLogLevel > 0) Debug.println(F(" parseHexChar == false"));
                }
                tmpUint8_1++;
              }
              if(ethInBuffer[4] == 'W' && ethInBuffer[5] == 'W') {
                if( Value * 10 > WWtMax) {
                  Value = WWtMax;
                  if (wwLogLevel > 0) {
                    Debug.print   ( F("WW: WWSpeicherTempSoll ueberschreitet max "));
                    Debug.println ( WWtMax);
                  }
                } 
                else {
                  Value *= 10;
                }
                if( setValue( _WWSpeicherTempSoll, Value) && wwLogLevel > 1) {
                  Debug.print   ( F("WW: WWSpeicherTempSoll Zugewiesen: "));
                  Debug.println ( Values[_WWSpeicherTempSoll].ValueX10);
                }
              }
              else if (ethInBuffer[4] == 'H' && ethInBuffer[5] == 'K') {
                if ( Value * 10 > HKtMax - HKHysterese) {
                  Value = HKtMax - HKHysterese;
                  if (hkLogLevel > 0) {
                    Debug.print   ( F("HK: HKVorlaufTempSoll ueberschreitet max "));
                    Debug.println ( Value);
                  }
                }
                if ( setValue( _HKVorlaufTempSoll, Value * 10) && hkLogLevel > 1) {
                  Debug.print   ( F("HK: HKVorlaufTempSoll Zugewiesen: "));
                  Debug.println   ( Values[_HKVorlaufTempSoll].ValueX10);
                }
              }
            } 
            else if (ethInBufferCount > 6 
              && ethInBuffer[0] == 'O'
              && ethInBuffer[1] == 'W'
              && ethInBuffer[2] == 'A' 
              && ethInBuffer[5] == '=' ) {
              if (ethLogLevel > 1) Debug.println( F( " = OneWireAddresse"));
              tmpUint8_1 = 0;
              if (!parseHexChar(ethInBuffer[3], digit)) {
                if ( ethLogLevel > 0) Debug.println( F( "\neth: owAddr: 1. Stelle der Addresse nicht nummerisch!")); 
              }
              else {
                tmpUint8_1 = digit * 16;
                if (!parseHexChar(ethInBuffer[4], digit)) {
                  if (ethLogLevel > 0) Debug.println( F("\neth: owAddr: 2. Stelle der Addresse nicht nummerisch!")); 
                }
                else {
                  tmpUint8_1+= digit;
                }
                tmpUint8_2 = 6;
                HexCharIndex = 0;
                AddrByteIndex = 0;
                boolean AfterFirstChar = false;
                while(tmpUint8_2 < ethInBufferCount && AddrByteIndex < 8) {
                  if (ethLogLevel > 2) {
                    Debug.print( F ( "  ethInBuffer["));
                    Debug.print(tmpUint8_2);
                    Debug.print( F ( "] = ")); 
                    Debug.println(ethInBuffer[tmpUint8_2]);
                  }
                  if (parseHexChar(ethInBuffer[tmpUint8_2], digit)) {
                    AfterFirstChar=true;
                    if(HexCharIndex > 1) {
                      HexCharIndex = 0; 
                      AddrByteIndex++;
                    }
                    if (ethLogLevel > 2) {
                      Debug.print( F ( "  digit = ")); 
                      Debug.println(digit, DEC);
                    }
                    switch (HexCharIndex++){
                    case 0:  
                      Addr[AddrByteIndex] = digit;                 
                      break;
                    case 1:  
                      Addr[AddrByteIndex] *= 16;
                      Addr[AddrByteIndex] += digit;                 
                      break;
                    }
                    if (ethLogLevel > 2) {
                      Debug.print( F ( "  Addr["));
                      Debug.print(AddrByteIndex);
                      Debug.print( F ( "] = ")); 
                      Debug.print(Addr[AddrByteIndex],DEC);
                      Debug.print( F ( " = ")); 
                      Debug.println(Addr[AddrByteIndex],HEX);
                    }
                  } 
                  else {
                    if (ethLogLevel > 2) Debug.println( F( "  parseHexChar = false"));
                    if (AfterFirstChar) {
                      HexCharIndex = 0; 
                      AddrByteIndex++;
                    }
                  }
                  tmpUint8_2++;
                } 
                if (ethLogLevel > 2) Debug.println(AddrByteIndex);
                if( AddrByteIndex != 7) {
                  if (ethLogLevel > 0) Debug.println( F( "\neth: owAddr: Addresslänge stimmt nicht!")); 
                }
                else {
                  for (AddrByteIndex=0; AddrByteIndex < 8; AddrByteIndex++) {
                    if (EEPROM.read( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex) != Addr[AddrByteIndex]){
                      if (ethLogLevel > 0) {
                        Debug.print  ( F( "\neth:     Schreibe EEPROM(")); 
                        Debug.print  ( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex); 
                        Debug.print  ( F( ", ")); 
                        Debug.print  ( Addr[AddrByteIndex], HEX); 
                        Debug.println(F(")")); 
                      }
                      EEPROM.write( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex, Addr[AddrByteIndex]);
                    }
                  }
                  if (ethLogLevel > 1) Debug.println( F("\neth: owAddr: OK"));
                } 
              }
            }
          }
        }
        else { //Die Zeichenfolge ist noch nicht beendet
          if (ethInBufferCount < sizeof(ethInBuffer)) {//Es ist auch noch Platz im Buffer
            ethInBuffer[ethInBufferCount++]=c; 
          }
          else {
            if (ethLogLevel > 0) Debug.println( F( "eth: ethInBuffer-Groesse ueberschritten"));
            recording = 0;
          }
        }
      }
    }
  }
  if (!First) if (ethLogLevel > 1) Debug.println();

  if(!client.connected()) {     // if there's no net connection
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
      if (ethLogLevel > 0) Debug.println( F( "\neth: disconnecting."));
      client.stop();
    }
    if (ethLogLevel > 0) Debug.print( F( "eth: \nEthernet.maintain: "));
    if (ethLogLevel > 0) 
      Debug.println(Ethernet.maintain());   
    else 
      Ethernet.maintain();                         // Maybe we will get a new IP so lets do this when no client is connected
    static uint8_t connectionErrors = 0;
    if (client.connect( "hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
      if (ethLogLevel > 0) Debug.println(F("\neth: connected"));
      static boolean BootLog = true;
      client.println( F ( ""));
      if (BootLog) {
        client.println( F ( "{SteuerungBoot}"));
        BootLog == false;
      } 
      else {
        client.println( F ( "{SteuerungReconnect}"));
      }
      connectionErrors = 0;
    } 
    else {        // if you didn't get a connection to the server:
      if (ethLogLevel > 0) Debug.println( F( "\neth: connection failed"));
      if (connectionErrors++ > 200) {
        if (ethLogLevel > 0) Debug.println( F( "eth: Reset in 10 Sec"));
        //ToDo: ACHTUNG das stöhrt den Offlinebetrieb -> evl. im EEprom merken das wir resettet haben und wenn das so ist nicht mehr resetten
        //      Vielleicht reicht es auch nur dann zu resetten wenn schonmal eine Verbindung bestanden hat und die jetzt weg ist.
        delay(10000);
        asm volatile ("  jmp 0");  
      }
    }
  } 
  else {
    static uint8_t Task  = 0;
    static uint8_t Value = 0;
    switch (Task){
    case 0:
      static uint32_t lastOwAddrSync = millis() - owAddrSyncIntervall;
      if (millis() - lastOwAddrSync > owAddrSyncIntervall) { //jede Stunde Addressen neu abfragen
        if (Value >= sizeof(owArray)) {
          lastOwAddrSync = millis();
          Task++; 
          Value = 0;
        } 
        else{
          //Bsp: sending: {"<OWA1=".AttrVal("HKRuecklauf","ID","0 0 0 0 0 0 0 0").">" }
          //     returns: <OWA1=00 00 00 00 00 00 00 00> 
          strcpy_P(buffer, Values[owArray[Value]].Name);
          if (ethLogLevel > 1) Debug.print( F( "eth: OUT: { \"<OWA"));
          client.print( F( "{ \"<OWA"));
          if (Value < 16) {
            if (ethLogLevel > 1) Debug.print( F( "0"));
            client.print( F( "0"));
          }
          if (ethLogLevel > 1) Debug.print( Value, HEX);
          client.print( Value, HEX);
          if (ethLogLevel > 1) Debug.print( F( "=\".AttrVal(\""));
          client.print( F( "=\".AttrVal(\""));
          if (ethLogLevel > 1) Debug.print(buffer);
          client.print(buffer);
          if (ethLogLevel > 1) Debug.println( F( "\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }"));
          client.println( F( "\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }"));
          Value ++;
        }
      } 
      else Task++; 
      break;
    case 1:
      static uint32_t lastSollSync = millis() - SollSyncIntervall;
      if (millis() - lastSollSync > SollSyncIntervall) { //jede Stunde Addressen neu abfragen
        if (Value >= 2) {
          lastSollSync = millis();
          Task++; 
          Value = 0;
        } 
        else {//Sende: { "<SollHK=".Value("HKSollTempVorgabe").">" }
          if (Value == 0) {
            if (ethLogLevel > 1) Debug.println( F( "eth: OUT: { \"<SollHK=\".Value(\"HKSollTempVorgabe\").\">\" }"));
            client.println( F( "{ \"<SollHK=\".Value(\"HKSollTempVorgabe\").\">\" }"));
          } 
          else if ( Value == 1) {
            if (ethLogLevel > 1) Debug.println( F( "eth: OUT: { \"<SollWW=\".Value(\"WWSollTempVorgabe\").\">\" }"));
            client.println( F( "{ \"<SollWW=\".Value(\"WWSollTempVorgabe\").\">\" }"));
          }
          Value ++;
        }
      } 
      else Task++; 
      break;
    case 2:    
      if (Value >= sizeof(Values)/sizeof(data)) {
        Task++; 
        Value = 0;
      } 
      else{
        if ( Values[Value].Changed == 1 && Values[Value].ValueX10 != ValueUnknown) {
          Values[Value].Changed = 0;
          strcpy_P(buffer, Values[Value].Name);
          if (ethLogLevel > 1) Debug.print( F("eth: OUT: set "));  
          client.print( F( "set ")); 
          if (ethLogLevel > 1) Debug.print( buffer);  
          client.print( buffer); 
          if (ethLogLevel > 1) Debug.print( F(" "));  
          client.print( F( " ")); 
          if (ethLogLevel > 1) Debug.println( ( float)Values[Value].ValueX10 / 10, 1);
          client.println( ( float)Values[Value].ValueX10 / 10, 1);
        }
        Value ++;
      }
      break; 
    default:
      Task=0;
    }
  }
  lastConnected = client.connected();    // store the state of the connection for next time through the loop
}














