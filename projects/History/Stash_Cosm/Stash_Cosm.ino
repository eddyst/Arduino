#include <EtherCard.h>    // https://github.com/jcw/ethercard
 
byte DataAvail = 0;

// EtherCard
byte mymac[] = { 0x00,0x22,0xf9,0x01,0x93,0xce };
char website[] PROGMEM = "volkszaehler";
byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;
 
void setup () {
  if (ether.begin(sizeof Ethernet::buffer, mymac, 28) == 0)
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip); 
 
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
 
  ether.printIp("SRV: ", ether.hisip);
}
 
void loop () {
  ether.packetLoop(ether.packetReceive());
 
  // read data from CC128
//  if (currentcost.available()) {
//    cc_read();
//  }
   static unsigned long FR;
  if (FR<millis())
  { 
    DataAvail = 1;
    FR=millis()+10000;
  }

  // Keep track of our iterations
  static int count = 0;
  if (DataAvail) {
    cosm_post(count++);
    DataAvail = 0;
  }
}
 
void cosm_post(int itr) {
 
  byte sd = stash.create();
  stash.print("?value=");
  stash.print("Hallo");
  stash.save();
 
  // generate the header with payload - note that the stash size is used,
  // and that a "stash descriptor" is passed in as argument using "$H"
  Stash::prepare(PSTR("POST /Log.php$H HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: 0" "\r\n"
                      "\r\n"),
           sd, website);
  // send the packet - this also releases all stash buffers once done
  ether.tcpSend();
}
 

