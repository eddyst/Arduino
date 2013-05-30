//#define DebugOnNano
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
  Debug.print("freeMemory  = "); Debug.println( freeMemory());
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






