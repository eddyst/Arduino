//#define UseDHCPinSketch
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // MAC address.

#include <SPI.h>
#include <Ethernet.h>
EthernetClient client;

void EthInit() {
  Debug.println("Initialising the Ethernet controller");
  if (Ethernet.begin(mac) == 0) {
    Debug.println("Failed to configure Ethernet using DHCP");
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Debug.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Debug.print(Ethernet.localIP()[thisByte], DEC);
    Debug.print("."); 
  }
  Debug.println();
}
void ethDoEvents() {
  static boolean lastConnected = false;                 // state of the connection last time through the main loop
  boolean First = false;
  while (client.available()) {   // if there's incoming data from the net connection.
    char c = client.read();   
    if (First){    
      Debug.println();
      Debug.println("Client: "); 
      First=true;
    }
    Debug.print(c);              // send it out the Debug port. For debugging purposes
  }

  if(!client.connected()) {     // if there's no net connection
    if (lastConnected){         // but there was one last time through the loop, then stop the client:
      Debug.println();
      Debug.println("disconnecting.");
      EthState = 1;
      client.stop();
    }
    Debug.print("\nEthernet.maintain:");
    Debug.println(Ethernet.maintain());                            // Maybe we will get a new IP so lets do this when no client is connected
    static uint8_t connectionErrors = 0;
    if (client.connect("hm.fritz.box", 7072)) {    // try to get a connection, report back via serial:
      Debug.println();
      Debug.println("connected");
      connectionErrors = 0;
      EthState = 2;
      client.println("help");
    } 
    else {        // if you didn't get a connection to the server:
      Debug.println();
      Debug.println("connection failed");
      if (connectionErrors++ > 200) {
        Debug.println("Reset in 10 Sec");
        delay(10000);
        asm volatile ("  jmp 0");  
      }
      EthState = 3;
    }
  } 
  else {
    static uint8_t Task = 0;
    //{ "ThermeVorlaufTempVorgabe:".Value("ThermeVorlaufTempVorgabe") }
    if (Task++ >= sizeof(Values)/sizeof(data)) Task = 0;
    if ( Values[Task].Changed == 1 && Values[Task].ValueX10 != ValueUnknown) {
      Values[Task].Changed = 0;
      char buffer[30];
      strcpy_P(buffer, Values[Task].Name); // Necessary casts and dereferencing, just copy. 
      Debug.print( "\nset ");  
      Debug.print( buffer);  
      Debug.print( " ");  
      Debug.println( (float)Values[Task].ValueX10 / 10, 1);
      client.print(  "set "); 
      client.print( buffer); 
      client.print( " "); 
      client.println( (float)Values[Task].ValueX10 / 10, 1);
    }
  }
  lastConnected = client.connected();    // store the state of the connection for next time through the loop
}





