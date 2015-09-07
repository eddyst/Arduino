/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0x54, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,54, 54);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

int LEDpin = 4;
String readString = String(30);
String state = String(3);

void setup()
{
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  //Sets the LEDpin as an output
  pinMode(LEDpin,OUTPUT);
  
  digitalWrite(LEDpin,HIGH);
  state = "ON";
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if (readString.length() < 30) {
          readString.concat(c);
        }

        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          int LED = readString.indexOf("LED=");

          if (readString.substring(LED,LED+5) == "LED=T") {
            digitalWrite(LEDpin,HIGH);
            state = "ON";
          }
          else if (readString.substring(LED,LED+5) == "LED=F") {
            digitalWrite(LEDpin,LOW);
            state = "OFF";
          } 
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          client.print("LED is ");
          client.print(state);
          client.print("<br><br>");
          
          if (state == "ON") {
            client.println("<a href=\"./?LED=F\">Turn Off<a>");
          }
          else {
            client.println("<a href=\"./?LED=T\">Turn On<a>");
          }
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    readString = "";
    // close the connection:
    client.stop();
  }
}
