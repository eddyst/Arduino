#ifndef DebugOnNano
  //#define UseDHCPinSketch
  byte mac[] = { 0x00,0x22,0xf9,0x01,0x93,0xce };   // MAC address.
  prog_char website[] PROGMEM = "volkszaehler";
  
  EthernetClient client;
  boolean lastConnected = false;                 // state of the connection last time through the main loop

  void EthInit() {
    /* Check that the Ethernet controller exists
    ** Apparently the EN28J60 can get confused enough
    ** to require a reset also on DHCP-Refresh.
    */
    //Serial.println( F( "Initialising the Ethernet controller"));
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      for(;;)
        ;
    }
    // give the Ethernet shield a second to initialize:
    delay(1000);
  }
  
  void EthUpd(){
    char DataBuffer[14/*14={nl"Log":""nl}*/ + sizeof( logBuffer) + AddressCount * 8 /*6="##":,nl */ + AddressCountByteSum ];
    PString Data( DataBuffer, sizeof( DataBuffer));
   
    step0.blinkBegin( 1000, 0, 1);
    if (client.connect(website, 80)) {
      Serial.println("connecting...");
      // send the HTTP PUT request:
      Data.println( F( "{"));
      Data.print( F( "\"Log\":\""));
      Data.print( Log);
      Data.print( F( "\""));
      Log.begin();
      for ( uint8_t i = 0; i < AddressCount; i++) {
        if ( lastValueChanged[i] == true) {
          Data.println( F( ","));                      //Log.print( F( ","));
          Data.print( F( "\""));
          Data.print( toHex( addressTable[i][0]));     //Log.print( toHex( addressTable[i][0]));
          Data.print( toHex( addressTable[i][1]));     //Log.print( toHex( addressTable[i][1]));
          Data.print( F( "\":\""));                    //Log.print( F( "\":\""));
          uint8_t lVS = lastValueStart( i);
          for ( uint8_t b = lVS; b < lVS + addressTable[i][2]; b++) {
            Data.print( toHex( lastValue[b]));         //Log.print( toHex( lastValue[b]));
          }
          Data.print( F( "\""));                       //Log.print( F( "\""));
          lastValueChanged[i] = false;
        }
      }
      Data.print( F( "}"));                            //Log.print( F( "}"));
      client.println( F("POST /VitoR.php HTTP/1.0"));
      client.print  ( F("Host: "));
      client.println( website);
  //    client.println( F("Connection: close"));
  //    client.println( F("Content-Type: text/csv"));
      client.print  ( F("Content-Length: "));
      client.println( Data.length());
      client.println();
      client.println( Data);
    }
  }
  
  void ethDoEvents() {
    Ethernet.maintain();
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    if (!client.connected() && lastConnected) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
    }
  }
  void ethDoEvents2() {
    lastConnected = client.connected();
  }  
#else
  void EthInit() {
    Serial.begin( 57600);
  }
  
  void EthLog() {
    Serial.println( Log);
    Log.begin();
  }

  void EthUpd() {
    
  }
  
  void ethDoEvents() {
  }

#endif

