//#define UseDHCPinSketch
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // MAC address.
#define owAddrSyncIntervall 3600000


#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;

void EthInit() {
  Debug.println(F("eth: Initialising the Ethernet controller"));
  if (Ethernet.begin(mac) == 0) {
    Debug.println(F("eth: Failed to configure Ethernet using DHCP"));
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Debug.print(F("eth: My IP address: "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Debug.print(Ethernet.localIP()[thisByte], DEC);
    Debug.print("."); 
  }
  Debug.println();
}
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  boolean First = true;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();   
    if (First){    
      Debug.println();
      Debug.print(F("eth: IN: ")); 
      First=false;
    }
    Debug.print(c);              // send it out the Debug port. For debugging purposes
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
            Debug.println();
            Debug.print(F("eth: Zeichenfolge auswerten: ")); 
            for (int i = 0; i < ethInBufferCount; i++) {
              Debug.write(ethInBuffer[i]);
            }
            if (ethInBufferCount > 6 //&& ethInBuffer[0] == '<'
              && ethInBuffer[0] == 'O'
              && ethInBuffer[1] == 'W'
              && ethInBuffer[2] == 'A' 
              && ethInBuffer[5] == '=' ) {
              Debug.println(F(" = OneWireAddresse"));
              uint8_t digit;
              uint8_t owArrayIndex = 0;
              if (!parseHexChar(ethInBuffer[3], digit)) Debug.println(F(" 1. Stelle der Addresse nicht nummerisch!")); 
              else {
                owArrayIndex = digit * 16;
                if (!parseHexChar(ethInBuffer[4], digit)) Debug.println(F(" 2. Stelle der Addresse nicht nummerisch!")); 
                else {
                  owArrayIndex+= digit;
                }
                uint8_t i = 6, a = 0, b = 0, Addr[7];
                while(i < ethInBufferCount && b < 8) {
                  Debug.print("  ethInBuffer[");Debug.print(i);Debug.print("] = "); Debug.println(ethInBuffer[i]);
                  if (parseHexChar(ethInBuffer[i], digit)) {
                    if(a > 1) {
                      a = 0; 
                      b++;
                    }
                    Debug.print("  digit = "); Debug.println(digit, DEC);
                    switch (a++){
                    case 0:  
                      Addr[b] = digit;                 
                      break;
                    case 1:  
                      Addr[b] *= 16;
                      Addr[b] += digit;                 
                      break;
                    }
                    Debug.print("  Addr[");Debug.print(b);Debug.print("] = "); Debug.print(Addr[b],DEC);Debug.print(" = "); Debug.println(Addr[b],HEX);
                  } 
                  else {
                    Debug.println("  parseHexChar = false");
                    if (b > 0) {
                      a = 0; 
                      b++;
                    }
                  }
                  i++;
                } 
                Debug.println(b);
                if( b != 7) Debug.println(F(" Addresslänge stimmt nicht!")); 
                else {
                  for (a=0; a < 8; a++) {
                    if (EEPROM.read(owArrayIndex * 8 + a) != Addr[a]){
                      Debug.print(F("\neth:     Schreibe EEPROM(")); 
                      Debug.print( owArrayIndex * 8 + a); 
                      Debug.print(F(", ")); 
                      Debug.print( Addr[a], HEX); 
                      Debug.println(F(")")); 
                      EEPROM.write(owArrayIndex * 8 + a, Addr[a]);
                    }
                  }
                  Debug.println(F("eth: OK"));
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
            Debug.println( F("eth: ethInBuffer-Groesse ueberschritten"));
            recording = 0;
          }
        }
      }
    }
  }
  if (!First) Debug.println();
  
  if(!client.connected()) {     // if there's no net connection
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
      Debug.println();
      Debug.println(F("eth: disconnecting."));
      client.stop();
    }
    Debug.print(F("eth: \nEthernet.maintain: "));
    Debug.println(Ethernet.maintain());                            // Maybe we will get a new IP so lets do this when no client is connected
    static uint8_t connectionErrors = 0;
    if (client.connect("hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
      Debug.println();
      Debug.println(F("eth: connected"));
      connectionErrors = 0;
      client.println("");
    } 
    else {        // if you didn't get a connection to the server:
      Debug.println();
      Debug.println(F("eth: connection failed"));
      if (connectionErrors++ > 200) {
        Debug.println(F("eth: Reset in 10 Sec"));
        //ToDo: ACHTUNFG das stöhrt den Offlinebetrieb -> evl. im EEprom merken das wir resettet haben und wenn das so ist nicht mehr resetten
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
      static uint32_t lastOwAddrSync = millis() - owAddrSyncIntervall - 10;
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
          Debug.print("{ \"<OWA");
          if (Value < 16) Debug.print("0");
          Debug.print(Value,HEX);
          Debug.print("=\".AttrVal(\"");
          Debug.print(buffer);
          Debug.println("\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }");
          client.print("{ \"<OWA");
          if (Value < 16) client.print("0");
          client.print(Value,HEX);
          client.print("=\".AttrVal(\"");
          client.print(buffer);
          client.println("\",\"ID\",\"0 0 0 0 0 0 0 0\").\">\" }");
          Value ++;
        }
      } 
      else Task++; 
      break;
    case 1:    
      if (Value >= sizeof(Values)/sizeof(data)) {
        Task++; 
        Value = 0;
      } 
      else{
        if ( Values[Value].Changed == 1 && Values[Value].ValueX10 != ValueUnknown) {
          Values[Value].Changed = 0;
          strcpy_P(buffer, Values[Value].Name);
          Debug.print( F("eth: \nset "));  
          Debug.print( buffer);  
          Debug.print( F(" "));  
          Debug.println( (float)Values[Value].ValueX10 / 10, 1);
          client.print(  "set "); 
          client.print( buffer); 
          client.print( " "); 
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

































