void EthInit() {
  // initialize the ethernet device
  #if ethLogLevel > 1
    Debug.println( F( "eth: Initialising the Ethernet controller"));
  #endif
  #if useDHCP
    if (Ethernet.begin(mac) == 0) {
      #if ethLogLevel > 1
        Debug.println( F( "eth: Failed to configure Ethernet using DHCP"));
      #endif    
    }
    delay(1000);  // give the Ethernet shield a second to initialize:
    #if ethLogLevel > 1
      Debug.print( F( "eth: My IP address: "));
      Debug.println(Ethernet.localIP());
    #endif
  #else
    Ethernet.begin(mac, ip, dnsServer, gateway, subnet);
  #endif
  // start listening for clients
  server.begin();
}

boolean First;
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  First = true;
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {

    boolean newClient = true;
    for (byte i=0;i<4;i++) {
      //check whether this client refers to the same socket as one of the existing instances:
      if (clients[i]==client) {
        newClient = false;
        break;
      }
    }

    if (newClient) {
      //check which of the existing clients can be overridden:
      for (byte i=0;i<4;i++) {
        if (!clients[i] && clients[i]!=client) {
          clients[i] = client;
          // clead out the input buffer:
          client.flush();
          ethNewClient(i);
          break;
        }
      }
    }
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
    #if ethLogLevel > 1      
      if (!First) Debug.println();
    #endif
  }
  for (byte i=0;i<4;i++) {
    if (!(clients[i].connected())) {
      // client.stop() invalidates the internal socket-descriptor, so next use of == will allways return false;
      clients[i].stop();
    }
  }
  // Maybe we will get a new IP so lets do this when no client is connected
  //0: nothing happened
  //1: renew failed
  //2: renew success
  //3: rebind fail
  //4: rebind success
  #if ethLogLevel > 0
    #if ethLogLevel > 2 // 3 AlleInfos
        Debug.print( F( "eth: Ethernet.maintain 3: "));
        Debug.println(Ethernet.maintain());
    #else 
        byte EthMaintainResult = Ethernet.maintain();
      #if ethLogLevel > 1 // 2 Ein/Ausgehender Datenverkehr
        if (EthMaintainResult >0) {
          Debug.print( F( "eth: \nEthernet.maintain 2: "));
          Debug.println(EthMaintainResult);
        }
      #else // 1 Fehlermeldungen
        if ((EthMaintainResult == 1) || (EthMaintainResult == 3)) { // Fail
          Debug.print( F( "eth: \nEthernet.maintain 1: "));
          Debug.println(EthMaintainResult);
        }
      #endif
    #endif
  #else   //0 nix anzeigen
    Ethernet.maintain();                         
  #endif
}  

