#include <EtherCard.h>

#define BUF_SIZE 512
prog_char HostName[] PROGMEM = "Dach1";
byte mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };   // Nanode MAC address.
uint8_t ip[] = { 192, 168, 54, 52 };          // The fallback board address.
uint8_t dns[] = { 192, 168, 54, 1 };        // The DNS server address.
uint8_t gateway[] = { 192, 168, 54, 1 };    // The gateway router address.
uint8_t subnet[] = { 255, 255, 255, 0 };    // The subnet mask.
prog_char website[] PROGMEM = "volkszaehler";

int sensorPin=12;
int RelayPins[] = { 3, 2, 1, 0 };       // an array of pin numbers to which Relays are attached
int pinCount;
/*
typedef struct {
  byte pin;
  byte Value;
} VZmsg;
VZmsg SendQueue[10];
*/

byte Ethernet::buffer[BUF_SIZE];
Stash stash;
byte fixed;                                 // Address fixed, no DHCP
/*const char * DHCPstates[] = {
    "DHCP_STATE_BAD",
    "DHCP_STATE_INIT",
    "DHCP_STATE_SELECT",
    "DHCP_STATE_REQUEST",
    "DHCP_STATE_BOUND",
    "DHCP_STATE_RENEW",
    "DHCP_STATE_REBIND"
};
*/
unsigned long NextDoWork;

void setup(void)
{ Serial.begin(57600);
  Serial.println(F("---Setup---"));
  pinMode(sensorPin, INPUT);
  pinCount=sizeof(RelayPins)/sizeof(int);
  for (int i=0;i<pinCount;i++)
    { pinMode(RelayPins[i], OUTPUT);
      digitalWrite(RelayPins[i],LOW);
    }
  delay(2000);
 // Zum kalibrieren
 /*
 for (;;)
 { digitalWrite(0,HIGH); digitalWrite(3,LOW);
   delay(1000);
      Serial.print(analogRead(sensorPin));
   digitalWrite(0,LOW); digitalWrite(3,HIGH);
   delay(1000);
      Serial.println(analogRead(sensorPin));
 }
*/
//  Serial.println(ether.doBIST(28));
  EtherInit();
  Serial.println(F("---Setup End---"));
}

void loop(void)
{
//  Serial.print("DHCP state : ");
//  Serial.println(DHCPstates[ether.dhcpFSM()]);
  /* Check the DHCP lease timers, and update if necessary */
  if (!fixed)
  { ether.dhcpLease();
    if (!ether.dhcpValid())
      EtherInit();
  }   
  /* These function calls are required if ICMP packets are to be accepted */
  word rc = ether.packetLoop(ether.packetReceive());
  if (!rc==0)
  { Serial.print(F("ether.packetLoop() returned "));
    Serial.println(rc, DEC);
  }
  if (millis()>=NextDoWork)
  { DoWork();
  }
/*  static unsigned long FR;
  if (FR<millis())
  { Serial.print(F("FreeRam: "));
    Serial.println(freeRam());
    FR=millis()+5000;
  }
*/
  return;
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
void EtherInit()
{ /* Check that the Ethernet controller exists
  ** Apparently the EN28J60 can get confused enough
  ** to require a reset also on DHCP-Refresh.
  */
  Serial.println(F("Initialising the Ethernet controller"));
  if (ether.begin(sizeof Ethernet::buffer, mac, 28) == 0) 
    Serial.println(F("Ethernet controller NOT initialised"));
  else
  { /* Get a DHCP connection */
    for (int j = 3 ; true ; j--)
    { Serial.print(F("Attempting to get an IP address using DHCP ")); Serial.println(j);
      if (ether.dhcpSetup(HostName)) 
      { fixed = false;
        ether.printIp(F("Got an IP address using DHCP: "), ether.myip);
        Serial.print(F("DHCP lease time "));
        Serial.print(ether.dhcpLeaseTime(), DEC);
        Serial.println(F(" msec"));
        break;
      }
      else 
      if (j == 0)
      { /* If DHCP fails, start with a hard-coded address */
        ether.staticSetup(ip, gateway, dns);
        ether.printIp(F("DHCP FAILED, using fixed address: "), ether.myip);
        fixed = true;
        break; 
      }
      else
      { Serial.println(F("DHCP Retry in 1 Sec"));
        delay(1000);
      }
    }
    ether.printIp(F("Netmask: "), ether.mymask);
    ether.printIp(F("GW IP: "), ether.gwip);
    ether.printIp(F("DNS IP: "), ether.dnsip);
    ether.printIp("SRV: ", ether.hisip);
    if (!ether.dnsLookup(website)) 
      Serial.println("DNS failed"); 
    ether.printIp("SRV: ", ether.hisip);
  }
}


void DoWork()
{ static unsigned long NextCheckAll;
  static byte Status=0;
  static int LastValue[4];
  Serial.print(F("(Status/2)=")); Serial.print((Status/2));Serial.print(F("  %  (pinCount)=")); Serial.println((pinCount));
  byte Pin = (Status/2)%(pinCount);
  Serial.print(F("DoWork       Status ="));  Serial.print(Status); Serial.print(F(" Pin ="));  Serial.print(Pin);
  if (Status == 0)
    NextCheckAll = millis() + 600000;
  switch (Status % 2) 
  { case 0:
      Serial.println();
      for (int i=0; i<pinCount;i++)
        if (i==Pin)
        { Serial.print(F("<")); Serial.print(RelayPins[i]); Serial.print(F(">"));
        digitalWrite(RelayPins[i], HIGH);
        }
        else
        { Serial.print(F(" ")); Serial.print(RelayPins[i]); Serial.print(F(" ")); 
          digitalWrite(RelayPins[i], LOW);
        }
      Serial.println();
      Serial.println(F(" Status ++"));
      Status ++;
      NextDoWork = millis()+15000;
      break;
    case 1:
      // read the value from the sensor:
      int sensorValue = analogRead(sensorPin);
      Serial.print(F(" SensorValue=")); Serial.print(sensorValue);
      sensorValue = (sensorValue * 48828 - 7857143) * 35 / 1000000;
      Serial.print(F(" --> "));  Serial.print(sensorValue); Serial.print(F(" | "));  Serial.println(LastValue[Pin]);

     if (Status < pinCount * 2 or abs(LastValue[Pin]-sensorValue) > 1)
      { VZLog(Pin,sensorValue);
        LastValue[Pin] = sensorValue;
      }
     if (Status  < pinCount * 2 )
       { Serial.println(F(" Status ++"));
         Status ++;
       }
     else
       if (millis() > NextCheckAll)
         { Serial.println(F(" Status = 0"));
           Status = 0;
         }
       else
         { Serial.println(F(" SetNextWork"));
           NextDoWork = millis()+15000;
         }
     break;
    //default: 
  }
  Serial.print(F("             Status ="));  Serial.print(Status); 
  Serial.print(F("  millis()="));  Serial.println(millis());
  Serial.print(F("NextDoWork="));  Serial.println(NextDoWork);
}

void VZLog(int Pin, int Value)
{ Serial.print(F("VZLog"));
  byte sd = stash.create();
  switch (Pin)
  { case 0:
      stash.print(F("b7c8c160-0281-11e2-b5d2-134652b901b7")); break;
    case 1:
      stash.print(F("b435e240-0a08-11e2-997d-5f3fb9c0937a")); break;
    case 2:
      stash.print(F("c3443cc0-0a08-11e2-b2cd-0dcac0e09713")); break;
    case 3:
      stash.print(F("27e1bcb0-0a09-11e2-9040-7198e5fe9618")); break;   
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



/*
void VZLog(int Pin, int Value)
{
  ether.httpPost(PSTR("/middleware.php/data/"),NULL,"b7c8c160-0281-11e2-b5d2-134652b901b7.json?value=33.3 HTTP/1.1","ccccccc",my_result_cb);
// 192.168.54.118 - - [25/Sep/2012:21:49:05 +0200] "POST /middleware.php/data/ HTTP/1.1" 400 291 "-" "-"
// 192.168.54.118 - - [25/Sep/2012:21:49:06 +0200] "POST /middleware.php/data/ HTTP/1.1" 400 291 "-" "-"
}
static void my_result_cb (byte status, word off, word len) 
{ Serial.println(">>>"); 
  Ethernet::buffer[off+300] = 0; 
  Serial.print((const char*) Ethernet::buffer + off);
//  if(strstr((char*)Ethernet::buffer + off, "version")) 
//    Serial.println( "Good Card: LET THEM IN!!" ); 
//  else 
//    Serial.println( "Bad Card: GTFO!!" );
}
*/

 
