#define    ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define    aDCLogLevel 4   //0 nix, 1 Fehlermeldungen + unbekannte Addressen, 2 alle Buswechsel u. readings, 3 Statuswechsel, 4  Zuordnung + alles
#define Debug Serial

#include <SPI.h>
#include <Ethernet.h>
//Der Sketch verwendet 17.734 Bytes (57%) des Programmspeicherplatzes. Das Maximum sind 30.720 Bytes.
//Globale Variablen verwenden 523 Bytes (25%) des dynamischen Speichers, 1.525 Bytes für lokale Variablen verbleiben. Das Maximum sind 2.048 Bytes.

//#include <UIPEthernet.h>
//Der Sketch verwendet 25.108 Bytes (81%) des Programmspeicherplatzes. Das Maximum sind 30.720 Bytes.
//Globale Variablen verwenden 1.257 Bytes (61%) des dynamischen Speichers, 791 Bytes für lokale Variablen verbleiben. Das Maximum sind 2.048 Bytes.

EthernetClient client;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };   // MAC address.


const uint8_t pinCount = 1;
const unsigned long Intervall = 10000;
//int SensorPins[]   = {  A5,   A4,   A3,   A2};       // an array of pin numbers to which Relays are attached
int SensorPins[]   = {  A5};       // an array of pin numbers to which Relays are attached

uint32_t sample   [pinCount]; //Summe der 60 ADC-Werte
uint32_t LastValue[pinCount]; //
 int16_t LastTemp [pinCount];
uint32_t LastSend [pinCount];
byte Changed;

void setup()
{
  Debug.begin( 115200);
  Debug.println( F( "Setup"));
  for (uint8_t pin = 0; pin < pinCount; pin++) {
    LastSend[pin] = 0;
  }
  Debug.print( "sizeof(uint32_t) = ");
  Debug.println( sizeof(uint32_t));
  Debug.print( "sizeof(sample) = ");
  Debug.println( sizeof(sample));
  Debug.print( "sizeof(LastValue) = ");
  Debug.println( sizeof(LastValue));
  Debug.print( "sizeof(LastTemp) = ");
  Debug.println( sizeof(LastTemp));
  Debug.print( "sizeof(LastSend) = ");
  Debug.println( sizeof(LastSend));
  
  DoWorkInit();
  EthInit();
}

void EthSend() {
  for (uint8_t pin = 0; pin < pinCount; pin++) {
    if (   (bitRead( Changed, pin) == 1 )
           || (                               millis() - LastSend[pin] > 60000)) {
      LastSend[pin] = millis();
      if (ethLogLevel > 0) {
      Debug.print( F( "set KollektorValueDach"));
      Debug.print( pin + 1);
      Debug.print( F( " "));
      Debug.println(LastValue[pin]);
      Debug.print( F( "set KollektorDach"));
      Debug.print( pin + 1);
      Debug.print( F( " "));
      Debug.println(( float)LastTemp[pin] / 10, 1);      }
      client.print( F( "set KollektorValueDach"));
      client.print( pin + 1);
      client.print( F( " "));
      client.println(LastValue[pin]);
      client.print( F( "set KollektorDach"));
      client.print( pin + 1);
      client.print( F( " "));
      client.println(( float)LastTemp[pin] / 10, 1);
      bitClear(Changed, pin);
    }
  }
}

void loop()
{
  DoWork();
  EthSend();
  ethDoEvents();
}



