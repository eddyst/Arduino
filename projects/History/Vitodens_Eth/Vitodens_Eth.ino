
// !!!!!!!!!!!!!!!!! Funktioniert NICHT !!!!!!!!!!

#include <EtherCard.h>
#include <LEDTimer.h>

LEDTimer step0(0);
//int PinDnsLookupOK=3;

void setup() {
  step0.blink( 50, 200, 5);
  EthInit();
}

void loop() {
  ethDoEvents();

static unsigned long TimerEthSend;
  if (millis() - TimerEthSend > 10000) {
    EthLog();
    TimerEthSend = millis();
  }
}



  
