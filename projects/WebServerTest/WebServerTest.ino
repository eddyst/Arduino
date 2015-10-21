#include <UIPEthernet.h>
#include "pitches.h"
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0x14, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 54, 55);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

String readString = String(100); //string for fetching data from address

void setup()
{
  Serial.begin(115200);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  // start the Ethernet connection and the server:

  Serial.println(F("Initiaizing ethernet..."));

  // this uses a fixed address
  Ethernet.begin(mac, ip);

  // get an address with DHCP
  //if (Ethernet.begin(mac) == 0)
  //  Serial.println("Failed to configure Ethernet using DHCP");

  // give the card a second to initialize
  delay(1000);

  server.begin();

  Serial.print(F("Garage Door Opener Control Ready at IP address "));
  Serial.println(Ethernet.localIP());
}

void loop()
{
  // command received (one character)  '1' - activate garage door button
  char cmd = 0;          // 1 - pulse button
  boolean done = false;  // set to indicate that response is complete

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {

    Serial.println(F("new client"));
    readString = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //int i = c;
        //Serial.print("(");
        //Serial.print(i);
        //Serial.print(")");

        // store character received in receive string
        if (readString.length() < 100) {
          readString += (c);
        }

        // check for end of line
        if (c == '\n') {
          //Serial.print("Receved line: ");
          //Serial.print(readString);

          // process line if its the "GET" request
          // a request looks like "GET /?1" or "GET /?2"
          if (readString.indexOf("GET") != -1) {
            if (readString.indexOf("?1") != -1) {
              Serial.println(F("Lea DIM"));
              analogWrite(8, 50);
            } else if (readString.indexOf("?2") != -1) {
              Serial.println(F("Lea AN"));
              digitalWrite(8, HIGH);
            } else if (readString.indexOf("?3") != -1) {
              Serial.println(F("Lea AUS"));
              digitalWrite(8, LOW);
            } else if (readString.indexOf("?4") != -1) {
              Serial.println(F("Tim DIM"));
              analogWrite(9, 50);
            } else if (readString.indexOf("?5") != -1) {
              Serial.println(F("Tim AN"));
              digitalWrite(9, HIGH);
            } else if (readString.indexOf("?6") != -1) {
              Serial.println(F("Tim AUS"));
              digitalWrite(9, LOW);
            } else if (readString.indexOf("?7") != -1) {
              Serial.println(F("Sound"));
              int melody[] = {
                NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
              };

              // note durations: 4 = quarter note, 8 = eighth note, etc.:
              int noteDurations[] = {
                4, 8, 8, 4, 4, 4, 4, 4
              };
              for (int thisNote = 0; thisNote < 8; thisNote++) {

                // to calculate the note duration, take one second
                // divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                int noteDuration = 1000 / noteDurations[thisNote];
                tone(A3, melody[thisNote], noteDuration);

                // to distinguish the notes, set a minimum time between them.
                // the note's duration + 30% seems to work well:
                int pauseBetweenNotes = noteDuration * 1.30;
                delay(pauseBetweenNotes);
                // stop the tone playing:
                noTone(A3);
              }
            }
          }
          // if a blank line was received (just cr lf, length of 2), then its the end of the request
          if (readString.length() == 2) {
            // send web page back to client
            Serial.println(F("sending web page"));
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            // to specify the length, wooul have to construct the entire string and then get its length
            //client.println("Content-Length: 1234");
            client.println(F("Connnection: close"));
            client.println();

            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html>"));
            client.println(F("<head>"));
            client.println(F("<title>Nachtlampen</title>"));

            client.println(F("<style type='text/css'>"));
            client.println(F(".label {font-size: 30px; text-align:center;}"));
            client.println(F("button {width: 160px; height: 70px; font-size: 30px; -webkit-appearance: none; background-color:#dfe3ee; }"));
            client.println(F("</style>"));

            client.println(F("<script type='text/javascript'>"));

            client.println(F("function OnButtonClicked(parm) { window.location.href=\"X?\" + parm; }"));

            client.println(F("</script>"));

            client.println(F("</head>"));

            client.println(F("<body style=\"background-color:#3b5998\">"));

            client.println(F("<div class=\"label\">"));

            client.println(F("Nachtlampensteuerung<br/><br/>"));


            // door open / close button
            client.println(F("Lea<button onclick=\"OnButtonClicked('1');\">DIM</button>"));
            if (digitalRead(8) == LOW)
            {
              client.println(F("<button onclick=\"OnButtonClicked('2');\">AN</button><br/><br/>"));
            } else {
              client.println(F("<button onclick=\"OnButtonClicked('3');\">AUS</button><br/><br/>"));
            }
            client.println(F("Tim<button onclick=\"OnButtonClicked('4');\">DIM</button>"));
            if (digitalRead(9) == LOW)
            {
              client.println(F("<button onclick=\"OnButtonClicked('5');\">AN</button><br/><br/>"));
            } else {
              client.println(F("<button onclick=\"OnButtonClicked('6');\">AUS</button><br/><br/>"));
            }
            client.println(F("<button onclick=\"OnButtonClicked('7');\">Sound</button><br/><br/>"));

            // add more buttons here
            // button separator

            client.println(F("</div>"));

            client.println(F("</body>"));
            client.println(F("</html>"));

            client.println("");
            Serial.println(F("web page sent"));

            cmd = 0;

            // break out and disconnect. This will tell the browser the request is complete without   having to specify content-length
            break;

          }  // end of request reached

          // start line over
          readString = "";
        }  // end of line reached
      }  // end data is available from client
    }  // end cient is connected
    // give the web browser time to receive the data
    Serial.println(F("delay before disconnect"));
    delay(100);
    // close the connection:
    client.stop();
    Serial.println(F("client disonnected"));
  }  // end client has been created
}


