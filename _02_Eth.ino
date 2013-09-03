byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // MAC address.
#define owAddrSyncIntervall 3600000
#define SollSyncIntervall    600000

#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;

void EthInit() {
  if (ethLogLevel > 0) Debug.println( F("eth: Initialising the Ethernet controller"));
  if (Ethernet.begin(mac) == 0) {
    if (ethLogLevel > 0) Debug.println(F("eth: Failed to configure Ethernet using DHCP"));
  }
  delay(1000);  // give the Ethernet shield a second to initialize:
  if (ethLogLevel > 0) {
    Debug.print(F("eth: My IP address: "));
    for (byte i = 0; i < 4; i++) {

      Debug.print(Ethernet.localIP()[i], DEC); // print the value of each byte of the IP address:
      Debug.print("."); 
    }
    Debug.println();
  }
}
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  boolean First = true;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();   
    if (First){    
      if (ethLogLevel > 1) Debug.print(F("\neth: IN: ")); 
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
            if (ethLogLevel > 2) Debug.print(F("\neth: Zeichenfolge auswerten: ")); 
            for (int i = 0; i < ethInBufferCount; i++) {
              if (ethLogLevel > 2) Debug.write(ethInBuffer[i]);
            }
            if (ethInBufferCount > 6 //&& ethInBuffer[0] == '<'
              && ethInBuffer[0] == 'O'
              && ethInBuffer[1] == 'W'
              && ethInBuffer[2] == 'A' 
              && ethInBuffer[5] == '=' ) {
              if (ethLogLevel > 1) Debug.println(F(" = OneWireAddresse"));
              uint8_t digit;
              uint8_t owArrayIndex = 0;
              if (!parseHexChar(ethInBuffer[3], digit)) {
                if (ethLogLevel > 0) Debug.println(F("\neth: owAddr: 1. Stelle der Addresse nicht nummerisch!")); 
              }
              else {
                owArrayIndex = digit * 16;
                if (!parseHexChar(ethInBuffer[4], digit)) {
                  if (ethLogLevel > 0) Debug.println(F("\neth: owAddr: 2. Stelle der Addresse nicht nummerisch!")); 
                }
                else {
                  owArrayIndex+= digit;
                }
                uint8_t i = 6, HexCharIndex = 0, AddrByteIndex = 0, Addr[7];
                boolean AfterFirstChar = false;
                while(i < ethInBufferCount && AddrByteIndex < 8) {
                  if (ethLogLevel > 2) {
                    Debug.print("  ethInBuffer[");
                    Debug.print(i);
                    Debug.print("] = "); 
                    Debug.println(ethInBuffer[i]);
                  }
                  if (parseHexChar(ethInBuffer[i], digit)) {
                    AfterFirstChar=true;
                    if(HexCharIndex > 1) {
                      HexCharIndex = 0; 
                      AddrByteIndex++;
                    }
                    if (ethLogLevel > 2) {
                      Debug.print("  digit = "); 
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
                      Debug.print("  Addr[");
                      Debug.print(AddrByteIndex);
                      Debug.print("] = "); 
                      Debug.print(Addr[AddrByteIndex],DEC);
                      Debug.print(" = "); 
                      Debug.println(Addr[AddrByteIndex],HEX);
                    }
                  } 
                  else {
                    if (ethLogLevel > 2) Debug.println(F("  parseHexChar = false"));
                    if (AfterFirstChar) {
                      HexCharIndex = 0; 
                      AddrByteIndex++;
                    }
                  }
                  i++;
                } 
                if (ethLogLevel > 2) Debug.println(AddrByteIndex);
                if( AddrByteIndex != 7) {
                  if (ethLogLevel > 0) Debug.println(F("\neth: owAddr: Addresslänge stimmt nicht!")); 
                }
                else {
                  for (AddrByteIndex=0; AddrByteIndex < 8; AddrByteIndex++) {
                    if (EEPROM.read( EEPROM_Offset_owArray + owArrayIndex * 8 + AddrByteIndex) != Addr[AddrByteIndex]){
                      if (ethLogLevel > 0) {
                        Debug.print(F("\neth:     Schreibe EEPROM(")); 
                        Debug.print( EEPROM_Offset_owArray + owArrayIndex * 8 + AddrByteIndex); 
                        Debug.print(F(", ")); 
                        Debug.print( Addr[AddrByteIndex], HEX); 
                        Debug.println(F(")")); 
                      }
                      EEPROM.write( EEPROM_Offset_owArray + owArrayIndex * 8 + AddrByteIndex, Addr[AddrByteIndex]);
                    }
                  }
                  if (ethLogLevel > 1) Debug.println(F("\neth: owAddr: OK"));
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
            if (ethLogLevel > 0) Debug.println( F("eth: ethInBuffer-Groesse ueberschritten"));
            recording = 0;
          }
        }
      }
    }
  }
  if (!First) if (ethLogLevel > 1) Debug.println();

  if(!client.connected()) {     // if there's no net connection
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
      if (ethLogLevel > 0) Debug.println(F("\neth: disconnecting."));
      client.stop();
    }
    if (ethLogLevel > 0) Debug.print(F("eth: \nEthernet.maintain: "));
    if (ethLogLevel > 0) 
      Debug.println(Ethernet.maintain());   
    else 
      Ethernet.maintain();                         // Maybe we will get a new IP so lets do this when no client is connected
    static uint8_t connectionErrors = 0;
    if (client.connect("hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
      if (ethLogLevel > 0) Debug.println(F("\neth: connected"));
      connectionErrors = 0;
      client.println("");
    } 
    else {        // if you didn't get a connection to the server:
      if (ethLogLevel > 0) Debug.println(F("\neth: connection failed"));
      if (connectionErrors++ > 200) {
        if (ethLogLevel > 0) Debug.println(F("eth: Reset in 10 Sec"));
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
          if (ethLogLevel > 1) Debug.print("eth: OUT: { \"<OWA");
          client.print("{ \"<OWA");
          if (Value < 16) {
            if (ethLogLevel > 1) Debug.print("0");
            client.print("0");
          }
          if (ethLogLevel > 1) Debug.print(Value,HEX);
          client.print(Value,HEX);
          if (ethLogLevel > 1) Debug.print("=\".AttrVal(\"");
          client.print("=\".AttrVal(\"");
          if (ethLogLevel > 1) Debug.print(buffer);
          client.print(buffer);
          if (ethLogLevel > 1) Debug.println("\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }");
          client.println("\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }");
          Value ++;
        }
      } else Task++; 
      break;
    case 1:
      static uint32_t lastSollSync = millis() - SollSyncIntervall;
      if (millis() - lastSollSync > SollSyncIntervall) { //jede Stunde Addressen neu abfragen
        if (Value >= 2) {
          lastSollSync = millis();
          Task++; 
          Value = 0;
        } else {
          if (Value == 0) {//Sende: { "<HK=".Value("HKSollTempVorgabe").">" }
            if (ethLogLevel > 1) Debug.println("eth: OUT: { \"<HK=\".Value(\"HKSollTempVorgabe\").\">\" }");
            client.println("{ \"<HK=\".Value(\"HKSollTempVorgabe\").\">\" }");
          } 
          else if ( Value == 1) {
            if (ethLogLevel > 1) Debug.println("eth: OUT: { \"<WW=\".Value(\"WWSollTempVorgabe\").\">\" }");
            client.println("{ \"<WW=\".Value(\"WWSollTempVorgabe\").\">\" }");
          }
          Value ++;
        }
      } else Task++; 
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
          client.print(  "set "); 
          if (ethLogLevel > 1) Debug.print( buffer);  
          client.print( buffer); 
          if (ethLogLevel > 1) Debug.print( F(" "));  
          client.print( " "); 
          if (ethLogLevel > 1) Debug.println( (float)Values[Value].ValueX10 / 10, 1);
          client.println( (float)Values[Value].ValueX10 / 10, 1);
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











