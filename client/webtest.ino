/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,1,178);  // app server

// Set the static IP address to use.
IPAddress ip(192,168,1,177);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

int led = 13;

void setup() {
 // Open serial communications:
  Serial.begin(9600);
  
  pinMode (led, OUTPUT);
  // start the Ethernet connection:
  Serial.println("try to congifure using IP address");
  Ethernet.begin(mac, ip);
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop()
{
  query();

  getResponse();  

  processResponse();
  
  //Serial.println(r);

  delay(10000);
  Serial.println();
  Serial.println("disconnecting.");
  client.stop();
}

void query(){
   Serial.println("querying...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 9090)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /app/events?id=1 HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  } 
}

void getResponse(){
  // if there are incoming bytes available 
  // from the server, read them and print them:
  Serial.println("Get Response...");
  String response = "";
  
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '['){
        response = "";
        response += c;
        do {
          if (client.available()){
            c = client.read();
            response += c;
          }
        } while(c != ']');
      }
    }
  }
    Serial.print("Got response: ");
    Serial.println(response);
  //return response; 
}

void processResponse(){
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  tone(8, 523, 300);
  delay(2000);               // wait for a second
  noTone(8);
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);               // wait for a second
}


