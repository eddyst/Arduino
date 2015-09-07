#define    ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define   allgLogLevel 1
#define   vitoLogLevel 1
#define     owLogLevel 1   //0 nix, 1 Fehlermeldungen + unbekannte Addressen, 2 alle Buswechsel u. readings, 3 Statuswechsel + Zuordnung + alles
#define thermeLogLevel 1
#define     hkLogLevel 1
#define     wwLogLevel 1
#define  solarLogLevel 1
#define   kollLogLevel 1
#define Debug Serial

#include <EEPROM.h>
//#include <ByteBuffer.h>
//#include <PString.h>
#include <MemoryFree.h>

void setup() {
  Debug.begin( 57600);
  Debug.println( F ( "Setup"));
  AllgInit();
  EthInit();
  VitoInit();
  ThermeInit();
  HKInit();
  WWInit();
  SolarInit();
  KollektorInit();
  Debug.print( F( "freeMemory  = ")); 
  Debug.println( freeMemory());
  //  ATS_GetFreeMemory();
}

void loop() {
  AllgDoEvents();
  ethDoEvents();
  vitoDoEvents();
  oneWireDoEvents();
  WWDoEvents();
  HKDoEvents();
  ThermeDoEvents(); 
  KollektorDoEvents();
  SolarDoEvents();
}







