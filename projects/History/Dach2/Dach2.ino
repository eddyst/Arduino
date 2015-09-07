#define    ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define     owLogLevel 1   //0 nix, 1 Fehlermeldungen + unbekannte Addressen, 2 alle Buswechsel u. readings, 3 Statuswechsel, 4  Zuordnung + alles
#define Debug Serial

#include <SPI.h>
#include <UIPEthernet.h>
EthernetClient client;

byte mac[] = {
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };   // MAC address.


int sensorPin=12;
int RelayPins[]    = {   0,    1,    2,    3};       // an array of pin numbers to which Relays are attached
int LastValue[]    = {1258, 1258, 1258, 1258};
uint32_t lastSend[]= {   0,    0,    0,    0};
int pinCount;


byte Changed;

void setup()
{
  Debug.begin( 115200);
  Debug.println( F ( "Setup"));

  EthInit();
}
int32_t KollektorDach1 = 1258;

void EthSend() {
  for (byte Pin = 0; Pin++; Pin < pinCount) {
/*    if (   (bitRead(Changed,Pin) == 1 && millis() - lastSend[Pin] > 1000      )
        || (                          && millis() - lastSend[Pin] > 20*60*1000)) {
      lastSend[Pin] = millis();
      if (ethLogLevel > 1) {Debug.print( F( "eth: OUT: set KollektorDach"));Debug.print( Pin + 1);Debug.print( F( " "));Debug.println(( float)KollektorDach1 / 10, 1);}
      client.print( F( "set KollektorDach"));
      client.print( Pin + 1);
      client.print( F( " "));
      client.println(( float)KollektorDach1 / 10, 1);
      bitClear(Changed,Pin);
    }  
  */}
}

void loop()
{
  DoWork();
  ethDoEvents();
}



