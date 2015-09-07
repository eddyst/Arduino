#ifndef DebugOnNano
  //prog_char HostName[] PROGMEM = "Vitodens";
  byte mac[] = { 0x00,0x22,0xf9,0x01,0x93,0xce };   // MAC address.
  uint8_t ip[] = { 192, 168, 54, 53 };          // The fallback board address.
  uint8_t dns[] = { 192, 168, 54, 1 };        // The DNS server address.
  uint8_t gateway[] = { 192, 168, 54, 1 };    // The gateway router address.
  uint8_t subnet[] = { 255, 255, 255, 0 };    // The subnet mask.
  //prog_char website[] PROGMEM = "192.168.54.20";
  prog_char website[] PROGMEM = "volkszaehler";
  
  //#define UseDHCPinSketch
  #define BUF_SIZE 512

  byte Ethernet::buffer[BUF_SIZE];
  Stash stash;
  
  void EthInit() {
    /* Check that the Ethernet controller exists
    ** Apparently the EN28J60 can get confused enough
    ** to require a reset also on DHCP-Refresh.
    */
    //Serial.println(F("Initialising the Ethernet controller"));
    if (ether.begin(sizeof Ethernet::buffer, mac, 28) != 0) {
  #ifdef UseDHCPinSketch
      // Get a DHCP connection
      for (int j = 3 ; true ; j--)
      { //Serial.print(F("Attempting to get an IP address using DHCP ")); Serial.println(j);
  //      if (ether.dhcpSetup(HostName)) 
        if (ether.dhcpSetup()) 
        { //ether.printIp(F("Got an IP address using DHCP: "), ether.myip);
          //Serial.print(F("DHCP lease time "));
          //Serial.print(ether.dhcpLeaseTime(), DEC);
          //Serial.println(F(" msec"));
          break;
        }
        else 
        if (j == 0)
        { // If DHCP fails, start with a hard-coded address 
  #endif
          ether.staticSetup(ip, gateway, dns);
          //ether.printIp(F("DHCP FAILED, using fixed address: "), ether.myip);
  #ifdef UseDHCPinSketch
          break; 
        }
        else
        { //Serial.println(F("DHCP Retry in 1 Sec"));
          delay(1000);
        }
      }
  #endif
      //ether.printIp(F("Netmask: "), ether.mymask);
      //ether.printIp(F("GW IP: "), ether.gwip);
      //ether.printIp(F("DNS IP: "), ether.dnsip);
      //ether.printIp("SRV: ", ether.hisip);
      if (ether.dnsLookup(website))
        step0.blink( 2000);
      else
        step0.blink( 300, 200, 10);
      //Serial.println("DNS failed"); 
      //ether.printIp("SRV: ", ether.hisip);
    }
    //else
    //  Serial.println(F("Ethernet controller NOT initialised"));
  }
  
  void EthLog() {
//    step1.blinkBegin( 500, 0, 1);
//    digitalWrite(2,HIGH);
    byte sd = stash.create();
    stash.print(F("?value="));
//    stash.print(Log);
    stash.print("Hallo");
//    Log=String("");
    stash.save();
    Stash::prepare(PSTR("POST /Log.php$H HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "\r\n"
                      "$H"),
           sd, website, stash.size(), sd);
/*    Stash::prepare(PSTR("POST /Log.php$H HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "\r\n"), sd, website);
*/
    ether.tcpSend();
//    digitalWrite(2,LOW);
  }
  
/*  void EthUpd() {
    //AddressCount
    //uint8_t *addressTable[]
    //uint8_t lastValue[];
    //boolean lastValueChanged[];  
//    digitalWrite(2,HIGH);
    byte sd = stash.create();
    stash.println("{");
    for ( uint8_t i = 0; i < AddressCount; i++) {
      stash.print("\"");
      stash.print(addressTable[i][0], HEX);
      stash.print(addressTable[i][1], HEX);
      stash.print("\":\"");
      uint8_t lVS = lastValueStart(i);
      for ( uint8_t b = lVS; i < lVS + addressTable[i][2]; b++) {
        stash.print(lastValue[b], HEX);
      }
      stash.print("\"");
      if (i == AddressCount - 1)
        stash.println();
      else
        stash.println(",");
    }
    stash.print("}");
    stash.save();
    Stash::prepare(PSTR("POST /VitoR.php HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "\r\n"
                      "$H"),
           website, stash.size(), sd);
    ether.tcpSend();
//    digitalWrite(2,LOW);
  }
*/  
  void ethDoEvents() {
    ether.packetLoop(ether.packetReceive());   // These function calls are required if ICMP packets are to be accepted 
    //  word rc = ether.packetLoop(ether.packetReceive());
    //  if (!rc==0)
    //  { Log+="ether.packetLoop() returned ");
    //    //Serial.println(rc, DEC);
    //  }
  }
  
#else
  void EthInit() {
    Serial.begin(57600);
  }
  
  void EthLog() {
    Serial.println(Log);
    Log=String("");
  }

  void EthUpd() {
    
  }
  
  void ethDoEvents() {
  }

#endif

