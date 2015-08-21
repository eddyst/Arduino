#include <Controllino.h>
#include <SPI.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

int pin0 = CONTROLLINO_D0;
int pin1 = CONTROLLINO_D1;
volatile int state0 = LOW;
volatile int state1 = LOW;

#define PIN CONTROLLINO_IN1  // the pin we are interested in

void setup()
{
  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);

  pinMode(PIN, INPUT);     //set the pin to input
  digitalWrite(PIN, LOW); //do not use the internal pullup resistor
  PCintPort::attachInterrupt(PIN, blink0, RISING); // attach a PinChange Interrupt to our pin on the rising edge
// (RISING, FALLING and CHANGE all work with this library)
// and execute the function burpcount when that pin changes
}

void loop()
{
  digitalWrite(pin0, state0);
  digitalWrite(pin1, state1);
}

void blink0()
{
  state0 = !state0;
}

void blink1()
{
  state1 = !state1;
}

