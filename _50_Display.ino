/*
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
LCDKeypad lcd;

void lcdInit(){
  lcd.begin(16, 2);
  lcd.clear();
}

void lcdDoEvents(){
#define MaxMenu 1
  static int lastButton;
  int buttonPressed;
  buttonPressed=lcd.button();
  if (buttonPressed != lastButton) {
    lastButton=buttonPressed;
    if (buttonPressed==KEYPAD_DOWN && Menu < MaxMenu) {
      Menu++;
    } 
    else if (buttonPressed==KEYPAD_UP && Menu > 0) {
      Menu--;
    }
  }
  switch (Menu){
  case 0:
    lcd.setCursor(0,0);
    switch (EthState) {
    case 0: 
      lcd.print ("-"); 
      break; 
    case 1: 
      lcd.print ("d"); 
      break;
    case 2: 
      lcd.print ("C"); 
      break;
    case 3: 
      lcd.print ("F"); 
      break;
    }
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      lcd.print(Ethernet.localIP()[thisByte], DEC);
      lcd.print("."); 
    }
    break;
  case 1:
    static uint32_t lastDateToDisplay = millis();
    if (millis() - lastDateToDisplay > 1000)
    {
      lastDateToDisplay = millis();
      DateTime dt = g_Clock.GetDateTime();
      lcd.setCursor(0,0);
      if (dt.Day < 10) { 
        lcd.print(("0"));
      }
      lcd.print(dt.Day);
      lcd.print(("."));
      if (dt.Month < 10) { 
        lcd.print(("0"));
      }
      lcd.print(dt.Month);
      lcd.print(("."));      
      lcd.print(dt.Year); 
      lcd.setCursor(0,1);
      if (dt.Hour   < 10) { 
        lcd.print("0");
      }
      lcd.print(dt.Hour);
      lcd.print(":");
      if (dt.Minute < 10) { 
        lcd.print("0");
      }
      lcd.print(dt.Minute);
      lcd.print(":");
      if (dt.Second < 10) { 
        lcd.print("0");
      }
      lcd.print(dt.Second);
      lcd.print(" ");
    }
    break;
  }
}
*/



