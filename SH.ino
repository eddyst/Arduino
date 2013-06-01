#define  ethLogLevel 1   //0 nix, 1 Fehlermeldungen, 2 Ein/Ausgehender Datenverkehr, 3 AlleInfos
#define vitoLogLevel 1
#define   owLogLevel 1
#define   wwLogLevel 1
#define   hkLogLevel 1
#define pumpLogLevel 1

#define Debug Serial

#include <EEPROM.h>
//#include <ByteBuffer.h>
//#include <PString.h>
#include <MemoryFree.h>
#include <Clock.h>
Clock g_Clock;

#include <avr/pgmspace.h>
#define myPROGMEM __attribute__((section(".progmem.data")))

void setup() {
  Debug.begin( 57600);
  Debug.println( "Setup");
  EthInit();
  vitoInit();
  wwInit();
  hkInit();
  pumpenInit();
  g_Clock.SetTimezoneOffset(1,0); // Setup the clock. In Germany we are 1 hours ahead of GMT 
  Debug.print("freeMemory  = "); 
  Debug.println( freeMemory());
  //  ATS_GetFreeMemory();
}

void loop() {
  ethDoEvents();
  vitoDoEvents();
  oneWireDoEvents();
  WWControlDoEvents();
  HKControlDoEvents();
  pumpenDoEvents();
  g_Clock.Maintain();
}







