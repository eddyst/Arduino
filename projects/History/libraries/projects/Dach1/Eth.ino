#ifndef DebugOnNano
#define UseDHCPinSketch
byte mac[] = { 
  0x74,0x69,0x69,0x2D,0x30,0x31 };   // MAC address.
uint8_t ip[] = { 
  192, 168, 54, 52 };          // The fallback board address.
uint8_t dns[] = { 
  192, 168, 54, 1 };        // The DNS server address.
uint8_t gateway[] = { 
  192, 168, 54, 1 };    // The gateway router address.
uint8_t subnet[] = { 
  255, 255, 255, 0 };    // The subnet mask.
//prog_char HostName[] PROGMEM = "Dach1";
prog_char website[] PROGMEM = "volkszaehler";

#define BUF_SIZE 512

byte Ethernet::buffer[BUF_SIZE];
Stash stash;

void EthInit() {
  /* Check that the Ethernet controller exists
   ** Apparently the EN28J60 can get confused enough
   ** to require a reset also on DHCP-Refresh.
   */
  Serial.println( F( "Initialising the Ethernet controller"));
  if ( ether.begin( sizeof Ethernet::buffer, mac, 28) != 0) {
#ifdef UseDHCPinSketch
    // Get a DHCP connection
    for ( int j = 3 ; true ; j--)
    { 
      Serial.print( F( "Attempting to get an IP address using DHCP ")); 
      Serial.println( j);
      //      if ( ether.dhcpSetup( HostName)) 
      if ( ether.dhcpSetup()) 
      { 
        ether.printIp( F( "Got an IP address using DHCP: "), ether.myip);
//        Serial.print( F( "DHCP lease time "));
//        Serial.print( ether.dhcpLeaseTime(), DEC);
//        Serial.println( F( " msec"));
        break;
      }
      else 
        if ( j == 0)
      { // If DHCP fails, start with a hard-coded address 
#endif
        ether.staticSetup( ip, gateway, dns);
        ether.printIp( F( "DHCP FAILED, using fixed address: "), ether.myip);
#ifdef UseDHCPinSketch
        break; 
      }
      else {
        Serial.println( F( "DHCP Retry in 1 Sec"));
        delay( 1000);
      }
    }
#endif
    ether.printIp( F( "Netmask: "), ether.mymask);
    ether.printIp( F( "GW IP: "  ), ether.gwip);
    ether.printIp( F( "DNS IP: " ), ether.dnsip);
    ether.printIp( F( "SRV: "    ), ether.hisip);
    if ( ether.dnsLookup( website))
      Serial.println( F( "DNS failed")); 
    else
      Serial.println( F( "DNS failed")); 
    ether.printIp( F( "SRV: "), ether.hisip);
  }
  else
    Serial.println( F( "Ethernet controller NOT initialised"));
}

void VZLog(int Pin, int Value) { 
  Serial.print(F("VZLog"));
  byte sd = stash.create();
  switch (Pin)
  { 
  case 0:
    stash.print(F("b7c8c160-0281-11e2-b5d2-134652b901b7")); 
    break;
  case 1:
    stash.print(F("b435e240-0a08-11e2-997d-5f3fb9c0937a")); 
    break;
  case 2:
    stash.print(F("c3443cc0-0a08-11e2-b2cd-0dcac0e09713")); 
    break;
  case 3:
    stash.print(F("27e1bcb0-0a09-11e2-9040-7198e5fe9618")); 
    break;   
  }  
  stash.print(F(".json?value="));
  stash.print(Value*.1,1);
  stash.save();
  //Stash::prepare(PSTR("POST /middleware.php/data/$H HTTP/1.1"), Vnr);
  Stash::prepare(PSTR("POST /middleware.php/data/$H HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  sd, website, stash.size(), sd);
  Serial.println(ether.tcpSend());
}

void ethDoEvents() {
  word rc = ether.packetLoop( ether.packetReceive());   // These function calls are required if ICMP packets are to be accepted 
  if ( !rc==0) {
    Serial.print  ( F( "packetLoop="));
    Serial.println( rc, DEC);
  }
}

#else
void EthInit() {
}

void ethDoEvents() {
}

void VZLog(int Pin, int Value) { 
  Serial.print( F("Pin: "  )); 
  Serial.print   (Pin  );
  Serial.print( F("Value: ")); 
  Serial.println (Value);
}
#endif


