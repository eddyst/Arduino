/*

 Udp NTP Client
 
 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket 
 For more on NTP time servers and the messages needed to communicate with them, 
 see http://en.wikipedia.org/wiki/Network_Time_Protocol
 
 created 4 Sep 2010 
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe
 
 This code is in the public domain.

 */
#define Debug Serial

#include <SPI.h>         
#include <Ethernet.h>
#include <Clock.h>

#include <LiquidCrystal.h>
#include <LCDKeypad.h>
LCDKeypad lcd;

#include <MemoryFree.h>

//#define UseDHCPinSketch
Clock g_Clock;
unsigned long g_ulLastPrint;


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xAE, 0xBE };   // MAC address.

unsigned int localPort = 8888;      // local port to listen for UDP packets

void EthInit() {
  Debug.println();
  Debug.println( F( "Initialising the Ethernet controller"));
  if (Ethernet.begin(mac) == 0) {
    Debug.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Debug.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Debug.print(Ethernet.localIP()[thisByte], DEC);
    Debug.print("."); 
  }
  Debug.println();

}

void setup() 
{
 // Open serial communications and wait for port to open:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print( F("NTP-Time"));
  Debug.begin(57600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Debug.print( freeMemory());
  EthInit();
  g_ulLastPrint = millis();
  
  // Setup the clock. In New Germany we are 1 hours ahead of GMT 
 // g_Clock.Setup(&Serial);
  g_Clock.SetTimezoneOffset(1,0); 
 }

void loop()
{
    g_Clock.Maintain();
    lcd.setCursor(9,1);
    lcd.print(millis());
    if (millis() - g_ulLastPrint > 1000)
    {
//      Debug.println( freeMemory());
      g_ulLastPrint = millis();
//      g_Clock.WriteDateTime(&Serial);
      DateTime dt = g_Clock.GetDateTime();
      lcd.setCursor(0,0);
      if (dt.Day < 10) { lcd.print(F("0"));}lcd.print(dt.Day);lcd.print(F("."));
      if (dt.Month < 10) { lcd.print(F("0"));}lcd.print(dt.Month);lcd.print(F("."));      
      lcd.print(dt.Year); 
      lcd.setCursor(0,1);
      if (dt.Hour < 10) { lcd.print(F("0"));}lcd.print(dt.Hour);lcd.print(F(":"));
      if (dt.Minute < 10) { lcd.print(F("0"));}lcd.print(dt.Minute);lcd.print(F(":"));
      if (dt.Second < 10) { lcd.print(F("0"));}lcd.print(dt.Second);lcd.print(F(" "));
    }
}
