

#include "Weak.h"

void pongEvent(uint8_t) __attribute__((weak));
void pongEvent(uint8_t) {}
void debugEvent(char Msg[]) __attribute__((weak));
void debugEvent(char Msg[]) {}

void Weak::ping(uint8_t x) {
	debugEvent("Pong");
	pongEvent(x+1);
}

