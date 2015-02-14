#define    ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define    aDCLogLevel 4   //0 nix, 1 Fehlermeldungen + unbekannte Addressen, 2 alle Buswechsel u. readings, 3 Statuswechsel, 4  Zuordnung + alles
#define Debug Serial

//#include <SPI.h>
//#include <Ethernet.h>
//Der Sketch verwendet 17.734 Bytes (57%) des Programmspeicherplatzes. Das Maximum sind 30.720 Bytes.
//Globale Variablen verwenden 523 Bytes (25%) des dynamischen Speichers, 1.525 Bytes für lokale Variablen verbleiben. Das Maximum sind 2.048 Bytes.

#include <UIPEthernet.h>
//Der Sketch verwendet 25.108 Bytes (81%) des Programmspeicherplatzes. Das Maximum sind 30.720 Bytes.
//Globale Variablen verwenden 1.257 Bytes (61%) des dynamischen Speichers, 791 Bytes für lokale Variablen verbleiben. Das Maximum sind 2.048 Bytes.

EthernetClient client;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };   // MAC address.


#define pinCount 1
unsigned long Intervall = 30000;
//int SensorPins[]   = {  A5,   A4,   A3,   A2};       // an array of pin numbers to which Relays are attached
int SensorPins[]   = {  A5};       // an array of pin numbers to which Relays are attached

uint32_t sample   [pinCount - 1]; //Summe der 60 ADC-Werte
uint32_t LastValue[pinCount - 1]; //
int      LastTemp [pinCount - 1];
int      LastSend [pinCount - 1];
byte Changed;

void setup()
{
  Debug.begin( 115200);
  Debug.println( F ( "Setup"));
  DoWorkInit();
  EthInit();
  for (int pin=0;pin<pinCount;pin++) { 
    LastSend[pin] = 0;
  }
}

void EthSend() {
  for (byte Pin = 0; Pin < pinCount; Pin++) {
    if (   (bitRead( Changed, Pin) == 1 )
        || (                               millis() - LastSend[Pin] > 60000)) {
      LastSend[Pin] = millis();
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



