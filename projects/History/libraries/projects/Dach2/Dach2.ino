#define    ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define    aDCLogLevel 2   //0 nix, 1 Fehlermeldungen + unbekannte Addressen, 2 alle Buswechsel u. readings, 3 Statuswechsel, 4  Zuordnung + alles
#define Debug Serial

#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };   // MAC address.


int PowerPin = A0;
int SensorPins[]   = {  A5,   A4,   A3,   A2};       // an array of pin numbers to which Relays are attached
int LastTemp[]    = {1258, 1258, 1258, 1258};
int LastValue[]    = {1258, 1258, 1258, 1258};
uint32_t lastSend[]= {   0,    0,    0,    0};
#define pinCount 1


byte Changed;

void setup()
{
  Debug.begin( 115200);
  Debug.println( F ( "Setup"));
  DoWorkInit();
  EthInit();
}

void EthSend() {
  for (byte Pin = 0; Pin < pinCount; Pin++) {
    if (   (bitRead( Changed, Pin) == 1 )
        || (                               millis() - lastSend[Pin] > 60000)) {
      lastSend[Pin] = millis();
      if (ethLogLevel > 0) {Debug.print( F( "eth: OUT: set KollektorDach"));Debug.print( Pin + 1);Debug.print( F( " "));Debug.println(( float)LastValue[Pin] / 10, 1);}
      client.print( F( "set KollektorDach"));
      client.print( Pin + 1);
      client.print( F( " "));
      client.println(( float)LastTemp[Pin] / 10, 1);
      client.print( F( "set KollektorValueDach"));
      client.print( Pin + 1);
      client.print( F( " "));
      client.println(( float)LastValue[Pin] / 10, 1);
      bitClear(Changed,Pin);
    }   
  }
}

void loop()
{
  DoWork();
  ethDoEvents();
}



