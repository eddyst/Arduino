#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define ZeitStart 1

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#include <estServo.h>
estServo servoKralle, servoArm, servoSchwenken;
uint8_t posArm = 150, posKralle = 0, posSchwenken = 30;

#define stateMin  0
#define stateSek  1
#define stateBew  2
#define stateNone 3
uint8_t state = stateNone;

void setup()
{ pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  pinMode(A1, INPUT_PULLUP);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hallo, Tim!");
  servoArm.attach(12 , 600, 1000);
  servoArm.write(posArm);
  servoKralle.attach(3, 500, 900);
  servoKralle.write(posKralle);
  servoSchwenken.attach(11, 700, 1600);
  servoSchwenken.write(posSchwenken);
  switchState();
  
}

uint32_t tim = millis() - 20000;

uint16_t laufzeit;
#define speed 10
#define speedKralle 0

void loop()
{
  if (millis() - tim > 10000) {
//    if (posArm == 0)
//      posArm = 180;
//    else
//      posArm = 0;
    if (posKralle == 0)
      posKralle = 180;
    else
      posKralle = 0;
//    if (posSchwenken == 0)
//      posSchwenken = 180;
//    else
//      posSchwenken = 0;
    tim = millis();
  }
  //  lcd.setCursor(0, 0);
  //    lcd.print(state);
  //  lcd.print(" ");
  //  lcd.print(posSchwenken);
  //  lcd.print(" ");
  //  lcd.print(servoSchwenken.read());
  //  lcd.print(" ");
  //  lcd.print(servoSchwenken.readMicroseconds());
  //  lcd.print(" ");
  //  lcd.print(" ");
  //  lcd.print( analogRead(A1));
  //  lcd.print("      ");
  servoDoEvents();

  int lcd_key = read_LCD_buttons();  // read the buttons function
  if (lcd_key != btnNONE)
  { if (lcd_key == btnSELECT)
    { switchState();
    } else
    { switch (state)
      { case stateMin:
          { ZeitUpDown(lcd_key, 60);
            lcd.setCursor(ZeitStart + 1, 1);// set the cursor to column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
            break;
          }
        case stateSek:
          { ZeitUpDown(lcd_key, 1);
            lcd.setCursor(ZeitStart + 3, 1);// set the cursor to column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
            break;
          }
      }
    }
  }
}

int read_LCD_buttons()                    //function for detection of pressed keypad button
{ static int lastButton = btnNONE;
  static uint32_t lastStart;
  int b;
  int adc_key_in = analogRead(A0);              // read the analog value from the sensor
  if (adc_key_in > 900) // We make this the 1st option for speed reasons since it will be the most likely result
  { lastButton = btnNONE;
    return btnNONE;
  }
  else if (adc_key_in < 195)
    b = btnSELECT;
  else if (adc_key_in < 380)
    b = btnDOWN;
  else
    b =  btnUP;
  if ( b == lastButton)
  { if (millis() - lastStart > 200)
    { lastStart = millis();
      return b;
    }
  } else
  { lastButton = b;
    lastStart = millis();
  }
  return btnNONE;
}

void ZeitUpDown( int lcd_key, uint8_t faktor)
{ switch (lcd_key)               // depending on which button was pushed, we perform an action
  { case btnUP:
      { if( laufzeit + faktor < 3600)
          laufzeit += faktor;
        else
          laufzeit = 3600;
        refreshZeit();
        break;
      }
    case btnDOWN:
      { if( laufzeit > faktor)
          laufzeit -= faktor;
        else
          laufzeit = 0;
        refreshZeit();
        break;
      }
  }
}

void switchState()
{ if (++state >= stateNone)
    state = stateMin;
  switch (state)
  { case stateMin:
    case stateSek:
      { refreshZeit();
        break;
      }
    default:
      { lcd.setCursor(ZeitStart, 1);
        lcd.print("???                            ");
        break;
      }
  }
}

void refreshZeit()
{ lcd.setCursor(ZeitStart, 1);// set the cursor to column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  uint16_t x = laufzeit / 60;
  if (x < 10)
    lcd.print(F(" "));
  lcd.print(x);
  lcd.print(F(":"));
  x = laufzeit % 60;
  if (x < 10)
    lcd.print(F("0"));
  lcd.print(x);
};

void servoDoEvents()
{ uint8_t pos;
  static uint32_t lastMove;
  if (millis() - lastMove > speed)
  { lastMove = millis();
    pos = servoArm.read();
    if (pos < posArm)
      servoArm.write( pos + 1);
    else if ( pos > posArm)
      servoArm.write( pos - 1);
    pos = servoSchwenken.read();
    if (pos < posSchwenken)
      servoSchwenken.write( pos + 1);
    else if (pos > posSchwenken)
      servoSchwenken.write( pos - 1);
  }
static uint32_t lastMoveKralle;
  if (millis() - lastMoveKralle > speedKralle)
  { lastMoveKralle = millis();  
    pos = servoKralle.read();
    if (pos < posKralle)
      servoKralle.write( pos + 1);
    else if (pos > posKralle)
      servoKralle.write( pos - 1);
  }
}
