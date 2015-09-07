#ifndef OneWireSlave_h
#define OneWireSlave_h

#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"       // for delayMicroseconds, digitalPinToBitMask, etc
#else
#include "WProgram.h"      // for delayMicroseconds
#include "pins_arduino.h"  // for digitalPinToBitMask, etc
#endif

#if defined(__AVR__)
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint8_t
#define IO_REG_ASM asm("r30")
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))

#elif defined(__MK20DX128__)
#define PIN_TO_BASEREG(pin)             (portOutputRegister(pin))
#define PIN_TO_BITMASK(pin)             (1)
#define IO_REG_TYPE uint8_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask)         (*((base)+512))
#define DIRECT_MODE_INPUT(base, mask)   (*((base)+640) = 0)
#define DIRECT_MODE_OUTPUT(base, mask)  (*((base)+640) = 1)
#define DIRECT_WRITE_LOW(base, mask)    (*((base)+256) = 1)
#define DIRECT_WRITE_HIGH(base, mask)   (*((base)+128) = 1)

#elif defined(__SAM3X8E__)
// Arduino 1.5.1 may have a bug in delayMicroseconds() on Arduino Due.
// http://arduino.cc/forum/index.php/topic,141030.msg1076268.html#msg1076268
// If you have trouble with OneWire on Arduino Due, please check the
// status of delayMicroseconds() before reporting a bug in OneWire!
#define PIN_TO_BASEREG(pin)             (&(digitalPinToPort(pin)->PIO_PER))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint32_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask)         (((*((base)+15)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+5)) = (mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+4)) = (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+13)) = (mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+12)) = (mask))
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif

#elif defined(__PIC32MX__)
#define PIN_TO_BASEREG(pin)             (portModeRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint32_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask)         (((*(base+4)) & (mask)) ? 1 : 0)  //PORTX + 0x10
#define DIRECT_MODE_INPUT(base, mask)   ((*(base+2)) = (mask))            //TRISXSET + 0x08
#define DIRECT_MODE_OUTPUT(base, mask)  ((*(base+1)) = (mask))            //TRISXCLR + 0x04
#define DIRECT_WRITE_LOW(base, mask)    ((*(base+8+1)) = (mask))          //LATXCLR  + 0x24
#define DIRECT_WRITE_HIGH(base, mask)   ((*(base+8+2)) = (mask))          //LATXSET + 0x28

#else
#error "Please define I/O register types here"
#endif

// You can exclude CRC checks altogether by defining this to 0
#ifndef ONEWIRESLAVE_CRC
#define ONEWIRESLAVE_CRC 1
#endif

// Select the table-lookup method of computing the 8-bit CRC
// by setting this to 1.  The lookup table no longer consumes
// limited RAM, but enlarges total code size by about 250 bytes
#ifndef ONEWIRESLAVE_CRC8_TABLE
#define ONEWIRESLAVE_CRC8_TABLE 0
#endif

#define FALSE 0
#define TRUE  1

#define EXTERNAL 1
#define PARASITE 0

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

#define ONEWIRE_NO_ERROR 0
#define ONEWIRE_READ_TIMESLOT_TIMEOUT 1
#define ONEWIRE_WRITE_TIMESLOT_TIMEOUT 2
#define ONEWIRE_WAIT_RESET_TIMEOUT 3
#define ONEWIRE_VERY_LONG_RESET 4
#define ONEWIRE_VERY_SHORT_RESET 5
#define ONEWIRE_PRESENCE_LOW_ON_LINE 6
#define ONEWIRE_READ_TIMESLOT_TIMEOUT_LOW 7
#define ONEWIRE_READ_TIMESLOT_TIMEOUT_HIGH 8

class OneWireSlave {
  private:
  	IO_REG_TYPE pin_bitmask;
    volatile IO_REG_TYPE *baseReg;
  	
    bool recvAndProcessCmd();
    uint8_t waitTimeSlot();
    uint8_t waitTimeSlotRead();
    uint8_t power;
    char rom[8];
    char scratchpad[9];
    char temp_scratchpad[3];
    char scratchpadtemperature[2];
  public:
    OneWireSlave(uint8_t pin);
    void init(unsigned char rom[8]);
    void setScratchpad(unsigned char scratchpad[9]);
    void setScratchpad_external(char temp_scratchpad[3]);
    void setPower(uint8_t power);
    void setTemperature(unsigned char scratchpadtemperature[2]);
    bool waitForRequest(bool ignore_errors);
    bool waitForRequestInterrupt(bool ignore_errors);
    bool waitReset(uint16_t timeout_ms);
    bool waitReset();
    bool presence(uint8_t delta);
    bool presence();
    bool search();
    bool duty();
    void setResolution(uint8_t resolution);
  	uint8_t getResolution();
    void attach44h (void (*)(void));
    uint8_t sendData(char buf[], uint8_t data_len);
    uint8_t recvData(char buf[], uint8_t data_len);
    void send(uint8_t v);
    uint8_t recv(void);
    void sendBit(uint8_t v);
    uint8_t recvBit(void);
#if ONEWIRESLAVE_CRC
    static uint8_t crc8(char addr[], uint8_t len);
#endif
    uint8_t errno;
};

#endif