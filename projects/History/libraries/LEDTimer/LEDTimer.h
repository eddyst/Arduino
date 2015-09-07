#ifndef LEDTimer_H
#define LEDTimer_H

#include "Arduino.h"
 
class LEDTimer{
  public:
    LEDTimer(uint8_t ledPin);
	~LEDTimer();
//    bool getState();
    void on(void);    
    void off(void);
//    void toggle();
    void blink(uint16_t onMs);
    void blink(uint16_t onMs, uint16_t offMs);
    void blink(uint16_t onMs, uint16_t offMs, uint8_t times);
    void blinkBegin( uint16_t onMs);
    void blinkBegin( uint16_t onMs, uint16_t offMs, uint8_t times);
	void doEvents( void);
	void onBlinkEnd(void (*dFunction)());
  private:
//	bool status;
	uint8_t pin;
	uint8_t times;
	uint32_t timer;
	uint16_t onMs;
	uint16_t offMs;
	void (*blinkEndCallback)();
};
#endif