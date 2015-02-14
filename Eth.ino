void EthInit() {
#if ethLogLevel > 1
  Debug.println( F( "eth: Initialising the Ethernet controller"));
#endif
  if (Ethernet.begin(mac) == 0) {
#if ethLogLevel > 1
    Debug.println( F( "eth: Failed to configure Ethernet using DHCP"));
#endif    
  }
  delay(1000);  // give the Ethernet shield a second to initialize:
#if ethLogLevel > 1
    Debug.print( F( "eth: My IP address: "));
    for (byte i = 0; i < 4; i++) {
      Debug.print(Ethernet.localIP()[i], DEC); // print the value of each byte of the IP address:
      Debug.print( F ( "."));
    }
    Debug.println();
#endif
}

boolean First;
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  First = true;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();
    if ( First){
#if ethLogLevel > 1      
      Debug.print( F("\neth: IN: "));
#endif      
      First=false;
    }
#if ethLogLevel > 1
    Debug.print(c);              // send it out the Debug port. For debugging purposes
#endif
  }
  if (!First) if (ethLogLevel > 1) Debug.println();

  if(client.connected()) {
    EthSend();
  } else {
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
#if ethLogLevel > 0 
      Debug.println( F( "\neth: disconnecting."));
#endif
      client.stop();
    }
    // Maybe we will get a new IP so lets do this when no client is connected
#if ethLogLevel > 0
    Debug.print( F( "eth: \nEthernet.maintain: "));
    Debug.println(Ethernet.maintain());
#else
    Ethernet.maintain();                         
#endif
    static uint8_t connectionErrors = 0;
    if (client.connect( "hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
      if (ethLogLevel > 0) Debug.println(F("\neth: connected"));
      static boolean BootLog = true;
      client.println( F ( ""));
      if (BootLog) {
#if ethLogLevel > 0
        Debug.print( F( "eth: Send: {SteuerungDachBoot}"));
#endif
        client.println( F ( "{SteuerungDachBoot}"));
        BootLog == false;
      }
      else {
#if ethLogLevel > 0
        Debug.print( F( "eth: Send: {SteuerungDachReconnect}"));
#endif
        client.println( F ( "{SteuerungDachReconnect}"));
      }
      connectionErrors = 0;
    }
    else {        // if you didn't get a connection to the server:
#if ethLogLevel > 0
      Debug.println( F( "\neth: connection failed"));
#endif
      if (connectionErrors++ > 200) {
#if ethLogLevel > 0
        Debug.println( F( "eth: Reset in 10 Sec"));
#endif
        //ToDo: ACHTUNG das stöhrt den Offlinebetrieb -> evl. im EEprom merken das wir resettet haben und wenn das so ist nicht mehr resetten
        //      Vielleicht reicht es auch nur dann zu resetten wenn schonmal eine Verbindung bestanden hat und die jetzt weg ist.
        delay(10000);
        asm volatile ("  jmp 0");
      }
    }
  }
  lastConnected = client.connected();    // store the state of the connection for next time through the loop
}  

