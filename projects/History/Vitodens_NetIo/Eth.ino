#ifndef DebugOnNano
  //#define UseDHCPinSketch
  byte mac[] = { 0x00,0x22,0xf9,0x01,0x93,0xce };   // MAC address.
  uint8_t ip[] = { 192, 168, 54, 53 };          // The fallback board address.
  uint8_t dns[] = { 192, 168, 54, 1 };        // The DNS server address.
  uint8_t gateway[] = { 192, 168, 54, 1 };    // The gateway router address.
  uint8_t subnet[] = { 255, 255, 255, 0 };    // The subnet mask.
  //prog_char HostName[] PROGMEM = "Vitodens";
  prog_char website[] PROGMEM = "volkszaehler";
  
  #define BUF_SIZE 512

  byte Ethernet::buffer[BUF_SIZE];
  Stash stash;
  
  void EthInit() {
    /* Check that the Ethernet controller exists
    ** Apparently the EN28J60 can get confused enough
    ** to require a reset also on DHCP-Refresh.
    */
    //Serial.println( F( "Initialising the Ethernet controller"));
    if ( ether.begin( sizeof Ethernet::buffer, mac, 28) != 0) {
  #ifdef UseDHCPinSketch
      // Get a DHCP connection
      for ( int j = 3 ; true ; j--)
      { //Serial.print( F( "Attempting to get an IP address using DHCP ")); Serial.println( j);
  //      if ( ether.dhcpSetup( HostName)) 
        if ( ether.dhcpSetup()) 
        { //ether.printIp( F( "Got an IP address using DHCP: "), ether.myip);
          //Serial.print( F( "DHCP lease time "));
          //Serial.print( ether.dhcpLeaseTime(), DEC);
          //Serial.println( F( " msec"));
          break;
        }
        else 
        if ( j == 0)
        { // If DHCP fails, start with a hard-coded address 
  #endif
          ether.staticSetup( ip, gateway, dns);
          //ether.printIp( F( "DHCP FAILED, using fixed address: "), ether.myip);
  #ifdef UseDHCPinSketch
          break; 
        }
        else
        { //Serial.println( F( "DHCP Retry in 1 Sec"));
          delay( 1000);
        }
      }
  #endif
      //ether.printIp( F( "Netmask: "), ether.mymask);
      //ether.printIp( F( "GW IP: "), ether.gwip);
      //ether.printIp( F( "DNS IP: "), ether.dnsip);
      //ether.printIp( F( "SRV: ", ether.hisip);
      if ( ether.dnsLookup( website))
        step0.blink( 2000);
      else
        step0.blink( 300, 200, 10);
      //Serial.println( F( "DNS failed"); 
      //ether.printIp( F( "SRV: ", ether.hisip);
    }
    //else
    //  Serial.println( F( "Ethernet controller NOT initialised"));
  }
  
  void EthLog() {
    step0.blinkBegin( 500, 0, 1);
    byte sd = stash.create();
    stash.print( Log);
    Log.begin();
    stash.save();
    Stash::prepare( PSTR( "POST /Log.php HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "\r\n"
                      "$H"),
            website, stash.size(), sd);
    ether.tcpSend();
  }
  
  void EthUpd(){
    step0.blinkBegin( 1000, 0, 1);
    byte sd = stash.create();
    boolean first = true;
    stash.println( F( "{"));
    stash.print( F( "\"Log\":\""));
    stash.print( Log);
    stash.print( F( "\""));
    Log.begin();
    for ( uint8_t i = 0; i < AddressCount; i++) {
      if ( lastValueChanged[i] == true) {
        stash.println( F( ","));                      //Log.print( F( ","));
        stash.print( F( "\""));
        stash.print( toHex( addressTable[i][0]));     //Log.print( toHex( addressTable[i][0]));
        stash.print( toHex( addressTable[i][1]));     //Log.print( toHex( addressTable[i][1]));
        stash.print( F( "\":\""));                    //Log.print( F( "\":\""));
        uint8_t lVS = lastValueStart( i);
        for ( uint8_t b = lVS; b < lVS + addressTable[i][2]; b++) {
          stash.print( toHex( lastValue[b]));         //Log.print( toHex( lastValue[b]));
        }
        stash.print( F( "\""));                       //Log.print( F( "\""));
        lastValueChanged[i] = false;
      }
    }
    stash.print( F( "}"));                            //Log.print( F( "}"));
    stash.save();
    Stash::prepare( PSTR( "POST /VitoR.php HTTP/1.0" "\r\n"
                        "Host: $F" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"), website, stash.size(), sd);
    ether.tcpSend();
  }
  
  void ethDoEvents() {
    word rc = ether.packetLoop( ether.packetReceive());   // These function calls are required if ICMP packets are to be accepted 
    if ( !rc==0) {
      Log.print  ( F( "packetLoop="));
      Log.println( rc, DEC);
    }
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

