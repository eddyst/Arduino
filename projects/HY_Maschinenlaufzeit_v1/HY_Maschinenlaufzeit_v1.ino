#define Debug Serial

#include <SPI.h>
#include <Ethernet.h>

EthernetClient client;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01 };   // MAC address.

const uint8_t pinCount = 1;
volatile uint32t zaehler1 = 0;

void setup() {
  // put your setup code here, to run once:
  Debug.begin( 115200);
  Debug.println( F( "Setup"));
  DoWorkInit();
  EthInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  DoWork();
  ethDoEvents();
}

void EthConnect(){
  static uint8_t connectionErrors = 0;
  if (client.connect( "hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
    if (ethLogLevel > 0) Debug.println(F("\neth: connected"));
    static boolean BootLog = true;
    client.println( F ( ""));
    if (BootLog) {
      #if ethLogLevel > 0
        Debug.print( F( "eth: Send: {SteuerungDachBoot}"));
      #endif
      client.println( F ( "{SteuerungDachBoot}"));
      BootLog == false;
    } else {
      #if ethLogLevel > 0
        Debug.print( F( "eth: Send: {SteuerungDachReconnect}"));
      #endif
      client.println( F ( "{SteuerungDachReconnect}"));
    }
    connectionErrors = 0;
  } else {        // if you didn't get a connection to the server:
    #if ethLogLevel > 0
      Debug.println( F( "\neth: connection failed"));
    #endif
    if (connectionErrors++ > 200) {
      #if ethLogLevel > 0
        Debug.println( F( "eth: Reset in 10 Sec"));
      #endif
      //ToDo: ACHTUNG das stöhrt den Offlinebetrieb -> evl. im EEprom merken das wir resettet haben und wenn das so ist nicht mehr resetten
      //      Vielleicht reicht es auch nur dann zu resetten wenn schonmal eine Verbindung bestanden hat und die jetzt weg ist. 
      delay(10000);
      asm volatile ("  jmp 0");
    }
  }
}    
