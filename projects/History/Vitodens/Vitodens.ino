//#define DebugOnNano

#include <SPI.h>
#include <Ethernet.h>
#include <ByteBuffer.h>
#include <LEDTimer.h>
#include <MemoryFree.h>
#include <PString.h>

enum pStatusEnm{
  None,
  WaitForConnectionOffer,
  NotInitialized,
  Initialized,
  WaitForAnswer
} 
pStatus=None;

ByteBuffer inputBuffer; //Opto: to hold incoming data
char logBuffer[100];
PString Log( logBuffer, sizeof( logBuffer));

//If like to move into Progmem use PROGMEM prog_uint8_t and implement something to read it back
#define AddressCount 20
#define AddressCountByteSum 52
uint8_t Address_00[] = { 
  0x25, 0x00, 0x16};
uint8_t Address_01[] = { 
  0x55, 0x5A, 0x02};
uint8_t Address_02[] = { 
  0xA3, 0x93, 0x02};
uint8_t Address_03[] = { 
  0x08, 0x10, 0x02};
uint8_t Address_04[] = { 
  0x25, 0x44, 0x02};
uint8_t Address_05[] = { 
  0x08, 0x08, 0x02};
uint8_t Address_06[] = { 
  0x08, 0x12, 0x02};
//uint8_t Address_[] = { 0x08, 0x14, 0x02}; = immer 20
uint8_t Address_08[] = { 
  0x55, 0x25, 0x02};
uint8_t Address_09[] = { 
  0x55, 0x27, 0x02};
uint8_t Address_10[] = { 
  0x0A, 0x10, 0x01};
uint8_t Address_11[] = { 
  0x08, 0x8A, 0x04};
uint8_t Address_12[] = { 
  0x08, 0x86, 0x04};
uint8_t Address_13[] = { 
  0x23, 0x23, 0x01};
uint8_t Address_14[] = { 
  0x0A, 0x82, 0x01};
uint8_t Address_15[] = { 
  0x23, 0x02, 0x01};
uint8_t Address_16[] = { 
  0x23, 0x03, 0x01};
uint8_t Address_17[] = { 
  0x65, 0x13, 0x01};
uint8_t Address_18[] = { 
  0x76, 0x60, 0x02};
uint8_t Address_19[] = { 
  0x76, 0x63, 0x02};
uint8_t Address_20[] = { 
  0xA3, 0x8F, 0x02};
//uint8_t Address_21[] = { 0x65, 0x15, 0x01};

const uint8_t *addressTable[AddressCount] = { 
  Address_00, Address_01, Address_02, Address_03, Address_04, Address_05, Address_06, Address_08, Address_09, 
  Address_10, Address_11, Address_12, Address_13, Address_14, Address_15, Address_16, Address_17, Address_18, Address_19,
  Address_20};
uint8_t lastValue[AddressCountByteSum];  //61 = Summe der 3. Felder aus Adresse_??                                 
boolean lastValueChanged[AddressCount];  

LEDTimer led04Gesendet( 1);
LEDTimer led05Empfangen( 4);
LEDTimer led06Empfangen( 5);
LEDTimer ledWaitForAnswer( 6);
LEDTimer step0( 0);
//LEDTimer step1( 1);

void setup() {
  Log.print( F( "g"));  
  Log.print( freeMemory());
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode( 2,OUTPUT);
  step0.blink( 50, 200, 10);
  digitalWrite( 2,LOW);
  ledWaitForAnswer.onBlinkEnd( OnLedWaitForAnswerEnd);
#ifdef DebugOnNano
  digitalWrite( 2,HIGH);
  //  led04Gesendet.onBlinkEnd( OnLed04GesendetEnd);
  //  led05Empfangen.onBlinkEnd( OnLed05EmpfangenEnd);
#endif  
  EthInit();
  OptoInit();
}

void loop() {
  //  if ( freeMemory()<1000) digitalWrite( 2,HIGH);
  ethDoEvents();
  step0.doEvents();
  ledWaitForAnswer.doEvents();
  led04Gesendet.doEvents();
  //  led05Empfangen.doEvents();

  static unsigned long TimerEthSend;
  if ( millis() - TimerEthSend > 15000) {
    Log.print( F( "w")); 
    Log.print( freeMemory());
    /*    static boolean sType = true;
     if ( sType) {
     EthLog();
     sType = false;
     }
     else{
     Log.print( F( "W"));
     */
    EthUpd();
    //      sType = true;
    //    }
    TimerEthSend = millis();
  }


  // -- Do Work --
  static uint8_t run=0;
  static uint8_t ByteNum = 0;
  //String LogCurrentDS;
  //boolean BigLog = false;
  switch ( pStatus) {
  case None:
    Log.print( F( "j"));
    inputBuffer.clear();
    pStatus = WaitForConnectionOffer;
    led04Gesendet.blinkBegin( 1000);
    optoWrite( 0x04);
    ledWaitForAnswer.blinkBegin( 60000);
    break;
  case WaitForConnectionOffer:
    if ( inputBuffer.getSize()) {
      uint8_t B = inputBuffer.get();
      if ( B == 0x05) {
        Log.print( F( "k"));
        pStatus = NotInitialized;
        led05Empfangen.blinkBegin( 1000);
        optoWrite( ( uint8_t[]){ 
          0x16, 0x00, 0x00        }
        , 3);
        ledWaitForAnswer.blinkBegin( 60000);
        break;
      }
      else {
        Log.print( F( "l")); 
        Log.print( B, HEX);
      }
    }
    break;
  case NotInitialized:
    if ( inputBuffer.getSize()) {
      uint8_t B = inputBuffer.get();
      if ( B == 0x06) {
        Log.print( F( "m"));
        pStatus = Initialized;
        led06Empfangen.blinkBegin( 1000);
        ledWaitForAnswer.blinkBegin( 60000);
        break;
      }
      else {
        Log.print( F( "n")); 
        Log.print( B, HEX);
      }
    }
    break;
  case Initialized:
    if ( Log.length() < 50) {
      if ( !( run++ < AddressCount)) {
        run=0;
      }
      Log.print( F( "o")); 
      Log.print( String( run));
      //      Log.print( F( "h")); Log.print( String( millis()));
      //      if ( BigLog) 
      //        Log.print( LogCurrentDS;
      //      else
      //        Log.print( F( "v";
      //      LogCurrentDS = "";
      //      BigLog = false;

      // Gerätekennung abfragen
      //    optoWrite( ( uint8_t[]){ 0x41, 0x05, 0x00, 0x01, 0x00, 0xF8, 0x02, 0x00}, 8);
      //    Senden    :    41 05 00 01 00 F8 02 00
      //    Empfangen : 06 41 07 01 01 00 F8 02 20 B8 DB
      //    Auswertung: 0x20B8 = V333MW1
      optoWrite( ( uint8_t[]){ 
        0x41, 0x05, 0x00, 0x01, addressTable[run][0], addressTable[run][1], addressTable[run][2], 
        ( 6 + addressTable[run][0] + addressTable[run][1] + addressTable[run][2]) % 256      }
      , 8);
      pStatus = WaitForAnswer;
      ByteNum=0;
    };
    ledWaitForAnswer.blinkBegin( 60000);
    break;
  case WaitForAnswer:
    //ToDo: Ma gucken was dahinter kommt. Ich erwarte: 
    //    41 Telegramm-Start-Byte
    //    05 Länge der Nutzdaten
    //    00 00 = Anfrage, 01 = Antwort, 03 = Fehler
    //    01 01 = ReadData, 02 = WriteData, 07 = Function Call
    //    XX XX 2 uint8_t Adresse der Daten oder Prozedur
    //    XX Anzahl der Bytes, die in der Antwort erwartet werden
    //    XX Prüfsumme = Summe der Werte ab dem 2 Byte (ohne 41)      
    if ( inputBuffer.getSize() > 0) {
      static uint8_t dataLength;
      uint8_t B = inputBuffer.get();
      switch ( ByteNum) {
      case 0: 
        if ( B == 0x06) {
          //Log.print( F( "p"));
        }
        else {
          Log.print( F( "P")); 
          Log.print( B, HEX); //          LogCurrentDS+="P"; LogCurrentDS += String( B, HEX); BigLog = true;
          pStatus = None;
        }
        break; 
      case 1: 
        if ( B == 0x41) {
          //Log.print( F( "q"));
        }
        else {
          Log.print( F( "Q")); 
          Log.print( B, HEX);
          pStatus = None;
        }
        break;
      case 2:
        //Log.print( F( "s")); Log.print( B, HEX);
        dataLength = B;
        break;
      case 3:
        //Log.print( F( "r")); Log.print( B, HEX);
        static uint8_t StateByte1;
        StateByte1 = B;
        break;
      case 4:
        //Log.print( F( "r")); Log.print( B, HEX);
        static uint8_t StateByte2;
        StateByte2 = B;
        break;
      case 5:
        //Log.print( F( "t")); Log.print( B, HEX);
        static uint8_t addressByte1;
        addressByte1 = B;
        if ( addressByte1 != addressTable[run][0]){     //Das ist nicht die Adresse nach der ich gefragt habe
          Log.print( F( "T")); 
          Log.print( run); 
          Log.print( F( "_")); 
          Log.print( addressTable[run][0], HEX);
          pStatus = None;
        }
        break;
      case 6:
        //Log.print( F( "u")); Log.print( B, HEX);
        static uint8_t addressByte2;
        addressByte2 = B;
        if ( addressByte2 != addressTable[run][1]){     //Das ist nicht die Adresse nach der ich gefragt habe
          Log.print( F( "U")); 
          Log.print( run); 
          Log.print( F( "_")); 
          Log.print( addressTable[run][1], HEX);
          pStatus = None;
        }
        break;
      case 7:
        //Log.print( F( "r")); Log.print( B, HEX);
        if ( B != addressTable[run][2]){     //Bitte klären warum die erwartete Anzahl Bytes nicht stimmt
          Log.print( F( "R")); 
          Log.print( run); 
          Log.print( F( "_")); 
          Log.print( addressTable[run][2], HEX);
          pStatus = None;
        }
        break;
      default:
        if ( ByteNum < dataLength + 3) {
          //Log.print( F( "r")); Log.print( B, HEX);
          uint8_t lVByte = lastValueStart( run) + ByteNum - 8;
          //          Log.print( F( "_")); Log.print( lVByte);
          if ( lastValue[lVByte] != B) {
            //Log.print( F( ">"));
            lastValue[lVByte] = B;
            lastValueChanged[run] = true;
          }
        }
        else {
          //Log.print( F( "r")); Log.print( B, HEX);
          //        if ( Prüfsumme falsch)
          //        BIGLog = true;
          //        lastValueChanged[run] = false;
          pStatus = Initialized;
        }
      }
      ByteNum ++;
    }
  }
  // store the state of the connection for next time through
  // the loop:
  ethDoEvents2();
}

uint8_t lastValueStart(byte addr){
  uint8_t x;
  //Log.print( F( "z(")); Log.print( addr);
  for( uint8_t i=0; i < addr ; i++) {
    x += addressTable[i][2];
    //Log.print( F( "_")); Log.print( x);
  }
  //Log.print( F( ")"));
  return x;
}

void OnLedWaitForAnswerEnd() {
  Log.print( F( "i"));
  pStatus = None;
}

#ifdef DebugOnNano
void OnLed04GesendetEnd()
{
  digitalWrite( 2,HIGH);
  Serial.println( F( "OnLed04GesendetEnd - Put 0x05"));
  inputBuffer.put( 0x05);
}
void OnLed05EmpfangenEnd()
{
  digitalWrite( 2,HIGH);
  Serial.println( F( "OnLed05EmpfangenEnd - Put 0x06"));
  inputBuffer.put( 0x06);
}
#endif

