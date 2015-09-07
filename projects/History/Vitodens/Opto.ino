#ifndef DebugOnNano
  void OptoInit() {
    inputBuffer.init(32);

    Serial1.begin(4800);
    //Arduino Mega 2560
      
    // AVR NetIO mit ATMega644
        UCSR0C =  (1<<UCSZ00)|(1<<UCSZ01)|(1<<UPM01)|(1<<USBS0); //ins UCSRC Register schreiben
    // AVR NetIO mit ATMega32
     //   UCSRC =  (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1)|(1<<UPM1)|(1<<USBS); //ins UCSRC Register schreiben
  }
  void optoWrite(uint8_t Byte) {
    Serial1.write(Byte);
  }
  void optoWrite(uint8_t *buffer, size_t size) {
    Serial1.write(buffer, size);
  }

 
   // SerialEvent occurs whenever a new data comes in the
   // hardware serial RX.  This routine is run between each
   // time loop() runs, so using delay inside loop can delay
   // response.  Multiple bytes of data may be available.
  void serialEvent() {
    while (Serial1.available()) {
  //    cli();
      inputBuffer.put(Serial1.read()); 
  //    sei();
    }
  }
#else
  void OptoInit() {
    inputBuffer.init(32);
    //    Serial.begin(57600); wird in Eth erledigt

  }
  
  void optoWrite(byte Byte) {
    Serial.println(Byte, HEX);
  }
  void optoWrite(uint8_t *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
      Serial.print( buffer[i], HEX); Serial.print( " ");
    }
    Serial.println();
  }
  
  void serialEvent() {
    while (Serial.available()) {
  //    cli();
      inputBuffer.put(Serial.read()); 
  //    sei();
    }
  }

#endif

