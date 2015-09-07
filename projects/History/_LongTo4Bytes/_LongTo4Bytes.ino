void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  uint8_t a[4];
  uint32_t Stagnation = 1371223112;
  Serial.print(Stagnation); Serial.print  ("   ");  Serial.println(Stagnation, BIN);
  #define EEPROM_Offset_Stagnation 0
  for (uint8_t i = 0; i < 4; i++) {
    uint32_t x = Stagnation >> (8 * i);
    Serial.print  ("   ");
    a[EEPROM_Offset_Stagnation + 3 - i] = (x) & 255;
    Serial.print  (" a[");
    Serial.print  (EEPROM_Offset_Stagnation + 3 - i);
    Serial.print  ("]=");
    Serial.print  (a[EEPROM_Offset_Stagnation + 3 - i]);
    Serial.print  ("   ");
    Serial.println(a[EEPROM_Offset_Stagnation + 3 - i],BIN);
  }
  Stagnation=0;
  for (uint8_t i = 0; i < 4; i++) {
    uint32_t x = (Stagnation << 8);
    Serial.println ("                        ");  Serial.println(x, BIN);
    Serial.print  (" a["); Serial.print  (EEPROM_Offset_Stagnation + i); Serial.print  ("]=");
    Serial.println (a[EEPROM_Offset_Stagnation + i],BIN);
    Stagnation = x | a[EEPROM_Offset_Stagnation + i];
    Serial.print(Stagnation); Serial.print  (" ");  Serial.println(Stagnation, BIN);
    Serial.print  ("------");
  }
  Serial.println(Stagnation);
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}
