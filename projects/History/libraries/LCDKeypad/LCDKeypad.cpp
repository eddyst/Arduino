/*
  LCDKeypad.cpp
*/
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


// include this library's description file

#include <LiquidCrystal.h>
#include "LCDKeypad.h"

LCDKeypad::LCDKeypad() : LiquidCrystal(8, 9, 4, 5, 6, 7)
{ _pin = A0;
}

LCDKeypad::LCDKeypad(uint8_t buttons) : LiquidCrystal(8, 9, 4, 5, 6, 7)
{ _pin = A0;
}

LCDKeypad::LCDKeypad(uint8_t buttons, uint8_t rs,             uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) : LiquidCrystal(rs,  enable, d0, d1, d2, d3)
{ _pin = A0;  
}

LCDKeypad::LCDKeypad(uint8_t buttons, uint8_t rs,             uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) : LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
{ _pin = A0;  
}

LCDKeypad::LCDKeypad(uint8_t buttons, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) : LiquidCrystal(rs, rw, enable, d0, d1, d2, d3)
{ _pin = A0;  
}

LCDKeypad::LCDKeypad(uint8_t buttons, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) :LiquidCrystal( rs,  rw, enable, d0,  d1, d2, d3, d4, d5, d6, d7)
{ _pin = A0;  
}

int LCDKeypad::button()
{
  static int NUM_KEYS=5;
  static int adc_key_val[5] ={  
    30, 150, 360, 535, 760     };
  int k, input;
  input=analogRead(_pin);
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {
      return k;
    }
  }
  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed
  return k;
}