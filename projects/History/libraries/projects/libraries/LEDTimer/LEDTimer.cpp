#include "LEDTimer.h"

//<<constructor>> setup the LED, make pin an OUTPUT
LEDTimer::LEDTimer(uint8_t ledPin){
	this->pin=ledPin;
	this->times=0;
	pinMode(this->pin,OUTPUT);
	off();
}

//<<destructor>>
LEDTimer::~LEDTimer(){/*nothing to destruct*/}
//    bool getState();

void LEDTimer::on(void){
    	digitalWrite(pin,HIGH);
//    	this->status=true;
		this->timer = millis();
    }
    
void LEDTimer::off(void){
    	digitalWrite(pin,LOW);
//    	this->status=false;
		this->timer = millis();
}
/*
void LEDTimer::toggle(void){
	status ? off() : on();
}
*/
void LEDTimer::blink( uint16_t onMs){
	blink( onMs, 0);
}
void LEDTimer::blink( uint16_t onMs, uint16_t offMs = 0){
	blink( onMs, offMs, 1);
}
void LEDTimer::blink( uint16_t onMs, uint16_t offMs = 0, byte times = 1){
	for (byte i=0; i<times; i++){
		on();
		delay(onMs);
		off();
		delay(offMs);
	}	
}

void LEDTimer::blinkBegin( uint16_t newonMs) {
	blinkBegin(newonMs, 0, 1);
}
void LEDTimer::blinkBegin( uint16_t newonMs, uint16_t newoffMs, uint8_t newtimes) {
	this->onMs = newonMs;
	this->offMs = newoffMs;
	this->times = newtimes * 2 - 1; //times * 2(on/off) - 1(instant on)
	on();
}
void LEDTimer::blinkDisable() {
	this->times = 0;
}

void LEDTimer::onBlinkEnd(void (*Func)()){
	blinkEndCallback = Func;
}

void LEDTimer::doEvents( void) {
	if (this->times > 0) {
		if (times & 1) {
			if ( millis() - this->timer >= this-> onMs) {
				off();
				times--;
			}
		}
		else{
			if ( millis() - this->timer >= this-> offMs) {
				on();
				times--;
			}
		}
		if (this->times == 0){
		    if(blinkEndCallback)
            {
                  blinkEndCallback();
            } 
		}
	}
}

