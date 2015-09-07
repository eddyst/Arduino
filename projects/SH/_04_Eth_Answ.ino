void ethDoAnswers() {
  boolean First = true;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();   
    if ( First){    
      if (ethLogLevel > 1) Debug.print( F("\neth: IN: ")); 
      First=false;
    }
    if (ethLogLevel > 1) Debug.print(c);              // send it out the Debug port. For debugging purposes
    static char ethInBuffer[30];
    static uint8_t recording = 0, ethInBufferCount = 0;
    if (c == '<'){
      if (recording == 0) ethInBufferCount = 0;//Es beginnt eine neue Interessante Zeichenfolge
      recording ++; 
    } 
    else {
      if (recording > 0) {//Wenn kein Empfang läuft können wir uns den Rest sparen
        if (c == '>') {
          recording --;
          if (recording == 0) {
            if (ethLogLevel > 2) Debug.print( F( "\neth: Zeichenfolge auswerten: ")); 
            for (int i = 0; i < ethInBufferCount; i++) {
              if (ethLogLevel > 2) Debug.write(ethInBuffer[i]);
            }
            if (ethInBufferCount > 6 
              && ethInBuffer[0] == 'd'
              && ethInBuffer[1] == 'p'
              //### 3 Zahlen
              && ethInBuffer[5] == '=' ) {
            //Bsp: sending: dp044get
            //     returns: <dp044=123> 
            //oder returns: <dp044=!> wenn keine Daten vorhanden sind, die aktuell genug sind (Definition im cmdalias auf FHEM)
              if (ethLogLevel > 1) Debug.println( F( " = ueber dpAskArray angefordert"));
              tmpUint16_1 = 0;
              if (!parseHexChar(ethInBuffer[2], digit)) {
                if ( ethLogLevel > 0) Debug.println( F( "\neth: dpAskArray: 1. Stelle der Addresse nicht nummerisch!")); 
              } else {
                tmpUint16_1 = digit * 100;
                if (!parseHexChar(ethInBuffer[3], digit)) {
                  if (ethLogLevel > 0) Debug.println( F("\neth: dpAskArray: 2. Stelle der Addresse nicht nummerisch!")); 
                } else {
                  tmpUint16_1 += digit * 10;
                  if (!parseHexChar(ethInBuffer[4], digit)) {
                    if (ethLogLevel > 0) Debug.println( F("\neth: dpAskArray: 3. Stelle der Addresse nicht nummerisch!")); 
                  } else {
                    tmpUint16_1 += digit;
                    if (ethInBuffer[6] == '!') {
                      Value = ValueUnknown;
                    } else {
                      tmpUint8_1 = 6; //einlesen beginnt an 6.Stelle
                      Value = 0;
                      while(tmpUint8_1 < ethInBufferCount) {
                        if (ethLogLevel > 2) {
                          Debug.print  ( F("  ethInBuffer["));
                          Debug.print  ( tmpUint8_1);
                          Debug.print  ( F("] = ")); 
                          Debug.print  ( ethInBuffer[tmpUint8_1]);
                        }
                        if (parseHexChar(ethInBuffer[tmpUint8_1], digit)) {//Wir können mit parseHexChar arbeiten weils nur 1 Stelle ist
                          Value = Value * 10 + digit;
                          if (ethLogLevel > 2) {
                            Debug.print  ( F("  Value = "));
                            Debug.println( Value);
                          }
                        } else {
                          if (ethLogLevel > 0) Debug.println(F(" parseHexChar == false"));
                          Value = ValueUnknown;
                          tmpUint8_1 = ethInBufferCount;
                        }
                        tmpUint8_1++;
                      }
                    }
                    switch (tmpUint16_1) {
                    case _WWSpeicherTempSoll:
                      if( Value > WWtMax) {
                        Value = WWtMax;
                        if (wwLogLevel > 0) {
                          Debug.print   ( F("WW: WWSpeicherTempSoll ueberschreitet max "));
                          Debug.println ( WWtMax);
                        }
                      } 
                      if( setValue( _WWSpeicherTempSoll, Value) && wwLogLevel > 1) {
                        Debug.print   ( F("WW: WWSpeicherTempSoll Zugewiesen: "));
                        Debug.println ( Values[_WWSpeicherTempSoll].ValueX10);
                      }
                      break;
                    case _HKVorlaufTempSoll: //Sonderbehandlung um Überschreitung des Max abzufangen
                      if ( Value > HKtMax - HKHysterese) {
                        Value = HKtMax - HKHysterese;
                        if (hkLogLevel > 0) {
                          Debug.print   ( F("HK: HKVorlaufTempSoll ueberschreitet max "));
                          Debug.println ( Value);
                        }
                      }
                      if ( setValue( _HKVorlaufTempSoll, Value) && hkLogLevel > 1) {
                        Debug.print   ( F("HK: HKVorlaufTempSoll Zugewiesen: "));
                        Debug.println   ( Values[_HKVorlaufTempSoll].ValueX10);
                      }
                      break;
                    default:
                      if ( setValue( tmpUint16_1, Value) && ethLogLevel > 1) {
                        Debug.print   ( F("eth: dp  ")); 
                        Debug.print   ( tmpUint16_1);
                        Debug.print   ( F(" Zugewiesen: "));
                        Debug.println   ( Values[tmpUint16_1].ValueX10);
                      }
                    }
                  }    // alle 3 Stellen nummerisch
                }
              }        //"dp###"= erkannt
            } else if (ethInBufferCount > 6 
              && ethInBuffer[0] == 'd'
              && ethInBuffer[1] == 'p' 
              //### 3 Zahlen
              && ethInBuffer[5] == 'i' 
              && ethInBuffer[6] == 'd' 
              && ethInBuffer[7] == '=' ) {
              if (ethLogLevel > 1) Debug.println( F( " = OneWireAddresse"));
              tmpUint16_1 = 0;
              if (!parseHexChar(ethInBuffer[2], digit)) {
                if ( ethLogLevel > 0) Debug.println( F( "\neth: owAddr: 1. Stelle der Addresse nicht nummerisch!")); 
              }
              else {
                tmpUint16_1 = digit * 100;
                if (!parseHexChar(ethInBuffer[3], digit)) {
                  if (ethLogLevel > 0) Debug.println( F("\neth: owAddr: 2. Stelle der Addresse nicht nummerisch!")); 
                }
                else {
                  tmpUint16_1 += digit * 10;
                  if (!parseHexChar(ethInBuffer[4], digit)) {
                    if (ethLogLevel > 0) Debug.println( F("\neth: owAddr: 3. Stelle der Addresse nicht nummerisch!")); 
                  }
                  else {
                    tmpUint16_1 += digit;
                    tmpUint8_2 = 8; //einlesen beginnt an 7.Stelle
                    HexCharIndex = 0;
                    AddrByteIndex = 0;
                    boolean AfterFirstChar = false;
                    while(tmpUint8_2 < ethInBufferCount && AddrByteIndex < 8) {
                      if (ethLogLevel > 2) {
                        Debug.print( F ( "  ethInBuffer["));
                        Debug.print(tmpUint8_2);
                        Debug.print( F ( "] = ")); 
                        Debug.println(ethInBuffer[tmpUint8_2]);
                      }
                      if (parseHexChar(ethInBuffer[tmpUint8_2], digit)) {
                        AfterFirstChar=true;
                        if(HexCharIndex > 1) {
                          HexCharIndex = 0; 
                          AddrByteIndex++;
                        }
                        if (ethLogLevel > 2) {
                          Debug.print( F ( "  digit=")); 
                          Debug.println(digit, DEC);
                        }
                        switch (HexCharIndex++){
                        case 0:  
                          Addr[AddrByteIndex] = digit;                 
                          break;
                        case 1:  
                          Addr[AddrByteIndex] *= 16;
                          Addr[AddrByteIndex] += digit;                 
                          break;
                        }
                        if (ethLogLevel > 2) {
                          Debug.print( F ( "  Addr["));
                          Debug.print(AddrByteIndex);
                          Debug.print( F ( "]=")); 
                          Debug.print(Addr[AddrByteIndex],DEC);
                          Debug.print( F ( "=")); 
                          Debug.println(Addr[AddrByteIndex],HEX);
                        }
                      } 
                      else {
                        if (ethLogLevel > 2) Debug.println( F( "  parseHexChar==false"));
                        if (AfterFirstChar) {
                          if (ethLogLevel > 2) Debug.println( F( "  HexCharIndex=0"));
                          HexCharIndex = 0; 
                          AddrByteIndex++;
                        }
                      }
                      tmpUint8_2++;
                    } 
                    if (ethLogLevel > 2) Debug.println(AddrByteIndex);
                    if( AddrByteIndex != 7) {
                      if (ethLogLevel > 0) Debug.println( F( "\neth: owAddr: Addresslänge stimmt nicht!")); 
                    }
                    else {// owAdresse speichern
                      for (tmpUint8_1 = 0; tmpUint8_1 < sizeof(owArray); tmpUint8_1++) {
                        if (owArray[tmpUint8_1] == tmpUint16_1) {
                          for (AddrByteIndex=0; AddrByteIndex < 8; AddrByteIndex++) {
                            if (EEPROM.read( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex) != Addr[AddrByteIndex]){
                              if (ethLogLevel > 0) {
                                Debug.print  ( F( "\neth:     Schreibe EEPROM(")); 
                                Debug.print  ( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex); 
                                Debug.print  ( F( ", ")); 
                                Debug.print  ( Addr[AddrByteIndex], HEX); 
                                Debug.println(F(")")); 
                              }
                              EEPROM.write( EEPROM_Offset_owArray + tmpUint8_1 * 8 + AddrByteIndex, Addr[AddrByteIndex]);
                            }
                          }
                        }
                      }
                      if (ethLogLevel > 1) Debug.println( F("\neth: owAddr: OK"));
                    } 
                  } // alle 3 Stellen nummerisch
                }
              }
            } else {
              if (ethLogLevel > 1) Debug.println( F( " = Unbekannte Zeichenfolge"));
            }
          }
        }
        else { //Die Zeichenfolge ist noch nicht beendet
          if (ethInBufferCount < sizeof(ethInBuffer)) {//Es ist auch noch Platz im Buffer
            ethInBuffer[ethInBufferCount++]=c; 
          }
          else {
            if (ethLogLevel > 0) Debug.println( F( "eth: ethInBuffer-Groesse ueberschritten"));
            recording = 0;
          }
        }
      }
    }
  }
  if (!First) if (ethLogLevel > 1) Debug.println();
}
  
