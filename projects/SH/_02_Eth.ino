byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // MAC address.
#define owAddrSyncIntervall     3600000
#define dpAskArraySyncIntervall   60000

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
uint8_t  HexCharIndex, AddrByteIndex, Addr[8];
int16_t Value;
boolean AskWait = false;
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  ethDoAnswers();
  static uint32_t lastMaintain = 0;
  if (millis() - lastMaintain > 60 * 60 * 1000) {
    lastMaintain = millis();
    ethDoMaintain();
  }
  if(client.connected()) {     // if there's a net connection
    static uint32_t lastEthDoAsk = 0;
    if (AskWait || millis() - lastEthDoAsk > 100) {
      lastEthDoAsk = millis();
      ethDoAsk();
    }
  } else {
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
      if (ethLogLevel > 0) Debug.println( F( "\neth: disconnecting."));
      client.stop();
    }
    static uint8_t connectionErrors = 0;
    if (client.connect( "hm.fritz.box", 7073)) {    // try to get a connection, report back via serial:
      if (ethLogLevel > 0) Debug.println(F("\neth: connected"));
      client.println( F ( "pwd"));
      static boolean BootLog = true;
      client.println( F ( ""));
      if (BootLog) {
        client.println( F ( "{SteuerungBoot}"));
        AskWait = true;
        BootLog = false;
      } 
      else {
        client.println( F ( "{SteuerungReconnect}"));
        AskWait = true;
      }
      connectionErrors = 0;
    } else {        // if you didn't get a connection to the server:
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
  lastConnected = client.connected();    // store the state of the connection for next time through the loop
}

void ethDoMaintain() {
    if (ethLogLevel > 0) Debug.print( F( "eth: \nEthernet.maintain: "));
    tmpUint16_1 = Ethernet.maintain();                         
    if (ethLogLevel >0){
      switch (tmpUint16_1) {
      case 0: 
        Debug.println(F("nothing happened"));
      case 1: 
        Debug.println(F("renew failed"));
      case 2: 
        Debug.println(F("renew success"));
      case 3: 
        Debug.println(F("rebind fail"));
      case 4: 
        Debug.println(F("rebind success"));
      }
    }
}














