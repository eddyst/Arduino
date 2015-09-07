/*
estBUS.cpp (formerly NewSoftSerial.cpp) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

// When set, _DEBUG co-opts pins 11 and 13 for debugging with an
// oscilloscope or logic analyzer.  Beware: it also slightly modifies
// the bit times, so don't rely on it too much at high baud rates
#define _DEBUG 0
#define _DEBUG_PIN1 11
#define _DEBUG_PIN2 13
// 
// Includes
// 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include <estBUS.h>

#if F_CPU == 16000000
 const int _bitLength = 50;
#else

#error This version of estBUS supports only 16 and 8MHz processors

#endif

//
// Statics
//
#define REZESSIV HIGH
#define DOMINANT !REZESSIV
estBUS *estBUS::active_object = 0;

char estBUS::_transmit_buffer[_SS_MAX_TX_BUFF];
volatile uint8_t estBUS::_transmit_buffer_tail = 0;
volatile uint8_t estBUS::_transmit_buffer_head = 0;

char estBUS::_receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t estBUS::_receive_buffer_tail = 0;
volatile uint8_t estBUS::_receive_buffer_head = 0;

//
// Debugging
//
// This function generates a brief pulse
// for debugging or measuring on an oscilloscope.
inline void DebugPulse(uint8_t pin, uint8_t count)
{
#if _DEBUG
  volatile uint8_t *pport = portOutputRegister(digitalPinToPort(pin));

  uint8_t val = *pport;
  while (count--)
  {
    *pport = val | digitalPinToBitMask(pin);
    *pport = val;
  }
#endif
}

//
// Private methods
//


// This function sets the current object as the "listening"
// one and returns true if it replaces another 
bool estBUS::listen()
{
  if (active_object != this)
  {
    _buffer_overflow = false;
    uint8_t oldSREG = SREG;
    cli();
    _receive_buffer_head = _receive_buffer_tail = 0;
    active_object = this;
    SREG = oldSREG;
    return true;
  }

  return false;
}

//
// The receive routine called by the interrupt handler
//
void estBUS::recv()
{

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Preserve the registers that the compiler misses
// (courtesy of Arduino forum user *etracer*)
  asm volatile(
    "push r18 \n\t"
    "push r19 \n\t"
    "push r20 \n\t"
    "push r21 \n\t"
    "push r22 \n\t"
    "push r23 \n\t"
    "push r26 \n\t"
    "push r27 \n\t"
    ::);
#endif  

  uint8_t d = 0;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (rx_pin_read())
  {
    // Wait approximately 1/2 of a bit width to "center" the sample
    delayMicroseconds(_bitLength );
    DebugPulse(_DEBUG_PIN2, 1);

    // Read each of the 8 bits
    for (uint8_t i=0x1; i; i <<= 1)
    {
      delayMicroseconds(_bitLength * 4);
      DebugPulse(_DEBUG_PIN2, 1);
      uint8_t noti = ~i;
      if (rx_pin_read())
        d |= i;
      else // else clause added to ensure function timing is ~balanced
        d &= noti;
    }

    // skip the stop bit
//    delayMicroseconds(_rx_delay_stopbit);
    DebugPulse(_DEBUG_PIN2, 1);

    // if buffer full, set the overflow flag and return
    if ((_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != _receive_buffer_head) 
    {
      // save new data in buffer: tail points to where byte goes
      _receive_buffer[_receive_buffer_tail] = d; // save new byte
      _receive_buffer_tail = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    } 
    else 
    {
#if _DEBUG // for scope: pulse pin as overflow indictator
      DebugPulse(_DEBUG_PIN1, 1);
#endif
      _buffer_overflow = true;
    }
  }

#if GCC_VERSION < 40302
// Work-around for avr-gcc 4.3.0 OSX version bug
// Restore the registers that the compiler misses
  asm volatile(
    "pop r27 \n\t"
    "pop r26 \n\t"
    "pop r23 \n\t"
    "pop r22 \n\t"
    "pop r21 \n\t"
    "pop r20 \n\t"
    "pop r19 \n\t"
    "pop r18 \n\t"
    ::);
#endif
}

void estBUS::tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

uint8_t estBUS::rx_pin_read()
{
  return *_receivePortRegister & _receiveBitMask;
}

//
// Interrupt handling
//

/* static */
inline void estBUS::handle_interrupt()
{
  if (active_object)
  {
    active_object->recv();
  }
}

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
  estBUS::handle_interrupt();
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect)
{
  estBUS::handle_interrupt();
}
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
  estBUS::handle_interrupt();
}
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect)
{
  estBUS::handle_interrupt();
}
#endif

//
// Constructor
//
estBUS::estBUS(uint8_t receivePin, uint8_t transmitPin) : 
  _buffer_overflow(false)
{ uint8_t port;
  //set transmitPin
    pinMode(transmitPin, OUTPUT);
    _transmitBitMask = digitalPinToBitMask(transmitPin);
    port = digitalPinToPort(transmitPin);
    _transmitPortRegister = portOutputRegister(port);
    tx_pin_write(REZESSIV);

  //set receivePin
    pinMode(receivePin, INPUT);
    digitalWrite(receivePin, HIGH);  // pullup for normal logic!
    _receivePin = receivePin;
    _receiveBitMask = digitalPinToBitMask(receivePin);
    port = digitalPinToPort(receivePin);
    _receivePortRegister = portInputRegister(port);
}

//
// Destructor
//
estBUS::~estBUS()
{
  end();
}

//
// Public methods
//

void estBUS::begin()
{
    if (digitalPinToPCICR(_receivePin))
    {
      *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
      *digitalPinToPCMSK(_receivePin) |= _BV(digitalPinToPCMSKbit(_receivePin));
    }

#if _DEBUG
  pinMode(_DEBUG_PIN1, OUTPUT);
  pinMode(_DEBUG_PIN2, OUTPUT);
#endif

  listen();
}

void estBUS::end()
{
  if (digitalPinToPCMSK(_receivePin))
    *digitalPinToPCMSK(_receivePin) &= ~_BV(digitalPinToPCMSKbit(_receivePin));
}


// Read data from buffer
int estBUS::read()
{
  if (!isListening())
    return -1;

  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
  _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}

int estBUS::available()
{ //ToDo: Ist Nachricht komplett? 
  //ToDo: Dürfen wir senden?
  Serial.print("TH");Serial.print(_transmit_buffer_head);
  Serial.print("TT");Serial.print(_transmit_buffer_tail);
  if (_transmit_buffer_head != _transmit_buffer_tail){
    // Read from "head"
    uint8_t d = _transmit_buffer[_transmit_buffer_head]; // grab next byte
    _transmit_buffer_head = (_transmit_buffer_head + 1) % _SS_MAX_TX_BUFF;
    Serial.print("b");
    delay(10);
    tx_write_byte(d);
    Serial.print("b!");
  }
  delay(10);
  if (!isListening())
    return 0;

  return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}

boolean  estBUS:: tx_write_byte( uint8_t b )
{
  uint8_t oldSREG = SREG;
//ToDo: reenable  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  Serial.print("y");
  delay(10);
  if (!tx_write_bit(REZESSIV)) return 0;
  if (!tx_write_bit(REZESSIV)) return 0;
  if (!tx_write_bit(REZESSIV)) return 0;
  //Wenn 3 Zeiteinheiten nix passiert ist, schreiben wir 1 Einheit dominant
  if (!tx_write_bit(DOMINANT)) return 0;
  
  for (byte mask = 0x01; mask; mask <<= 1)
  {
    if (b & mask) { // send 1
      if (!tx_write_bit(REZESSIV)) return 0;
      if (!tx_write_bit(DOMINANT)) return 0;
      } else { // send 0
      if (!tx_write_bit(DOMINANT)) return 0;
      if (!tx_write_bit(REZESSIV)) return 0;
    }
  }
  tx_pin_write(REZESSIV); // restore pin to natural state
  SREG = oldSREG; // turn interrupts back on
}

boolean  estBUS::tx_write_bit(uint8_t pin_state){
  uint32_t start=micros();
  boolean OK=true;
  tx_pin_write(pin_state);
  while( micros() - start < _bitLength) {
    if (rx_pin_read() != pin_state)
    OK=false;
  }
  return OK;
}

size_t estBUS::write(uint8_t b)
{
  // if buffer full, set the overflow flag and return
  if ((_transmit_buffer_tail + 1) % _SS_MAX_TX_BUFF != _transmit_buffer_head)
  {
    // save new data in buffer: tail points to where byte goes
    _transmit_buffer[_transmit_buffer_tail] = b; // save new byte
    _transmit_buffer_tail = (_transmit_buffer_tail + 1) % _SS_MAX_TX_BUFF;
    return 1;
  }
  else
  {
    #if _DEBUG // for scope: pulse pin as overflow indictator
    DebugPulse(_DEBUG_PIN1, 2);
    #endif
    _buffer_overflow = true;
    return 0;
  }
}

void estBUS::flush()
{
  if (!isListening())
    return;

  uint8_t oldSREG = SREG;
  cli();
  _transmit_buffer_head = _transmit_buffer_tail = 0;
  _receive_buffer_head = _receive_buffer_tail = 0;
  SREG = oldSREG;
}

int estBUS::peek()
{
  if (!isListening())
    return -1;

  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  return _receive_buffer[_receive_buffer_head];
}
