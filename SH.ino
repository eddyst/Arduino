//#define DebugOnNano
#define Debug Serial

#include <SPI.h>
#include <Ethernet.h>
//#include <ByteBuffer.h>
//#include <LEDTimer.h>
//#include <MemoryFree.h>
#include <PString.h>
#include <Servo.h> 
#include <Clock.h>
Clock g_Clock;

#include <avr/pgmspace.h>
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))

char logBuffer[100];
PString Log( logBuffer, sizeof( logBuffer));

//LEDTimer step0( 13);

void setup() {
  Debug.begin( 57600);

//  step0.blink( 1000, 500, 5);

  // Setup the clock. In New Germany we are 1 hours ahead of GMT 
  g_Clock.SetTimezoneOffset(1,0); 

  ledInit();
//  lcdInit();

  Log.print  ( "setup,");  
  Debug.print( "setup,");
//    Debug.println( freeMemory());
  vitoInit();
  EthInit();
  wwInit();
  hkInit();
  pumpenInit();
}


void loop() {
//  step0.doEvents();
  ethDoEvents();
  vitoDoEvents();
  oneWireDoEvents();
  WWControlDoEvents();
  HKControlDoEvents();
  pumpenDoEvents();
  g_Clock.Maintain();
  ledDoEvents();
//  lcdDoEvents();
}






