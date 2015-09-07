/*
  LCDKeypad.h
*/

// ensure this library description is only included once
#ifndef LCDKeypad_h
#define LCDKeypad_h

// library interface description
#define KEYPAD_NONE -1
#define KEYPAD_RIGHT 0
#define KEYPAD_UP 1
#define KEYPAD_DOWN 2
#define KEYPAD_LEFT 3
#define KEYPAD_SELECT 4

class LCDKeypad: public LiquidCrystal
{ private:
    uint8_t _pin;
    
  public:
    LCDKeypad();
		LCDKeypad(uint8_t buttons);
    LCDKeypad(uint8_t buttons, uint8_t rs,             uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
		LCDKeypad(uint8_t buttons, uint8_t rs,             uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    LCDKeypad(uint8_t buttons, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
    LCDKeypad(uint8_t buttons, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
 
    int button();
};

#endif

