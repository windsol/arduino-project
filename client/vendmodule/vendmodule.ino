
#include <SPI.h>
#include <Ethernet.h>

#define VEND_RELAY 12               // PIN for enabling vend relay
#define RELAY_1 11                  // PIN for enabling column 1 relay
#define RELAY_2 10                  // PIN for enabling column 2 relay
#define RELAY_3 9                  // PIN for enabling column 3 relay
#define RELAY_4 8                  // PIN for enabling column 4 relay
#define RELAY_5 7                  // PIN for enabling column 5 relay
#define RELAY_6 6                  // PIN for enabling column 6 relay
#define RELAY_7 5                  // PIN for enabling column 7 relay
#define RELAY_8 4                  // PIN for enabling column 8 relay

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,1,178);  // app server

// Set the static IP address to use.
IPAddress ip(192,168,1,177);

// ID of the Vending Machine where this module is installed
String moduleId  = "1";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup()
{
  // Set all relay pins as OUTPUT
  pinMode(VEND_RELAY, OUTPUT);          // tells arduino RELAY is an output
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  pinMode(RELAY_5, OUTPUT);
  pinMode(RELAY_6, OUTPUT);
  pinMode(RELAY_7, OUTPUT);
  pinMode(RELAY_8, OUTPUT);
         
  // Open serial communications:
  Serial.begin(9600);

  // start the Ethernet connection:
  Serial.println("try to congifure using IP address");
  Ethernet.begin(mac, ip);
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop()
{
  if ( query() == 1) {
    
    String r = getResponse();
    
    String event = processResponse(r);
    if (event != ""){
       processEventString(event);
    }
  }
  delay(10000);
}

int query(){
   int queried = 0;
   Serial.println("Querying...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 9090)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /app/events?a=q&id=" + moduleId + " HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
    queried = 1;
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  } 
  return queried;
}

String getResponse(){
  // if there are incoming bytes available 
  // from the server, read them and print them:
  String response = ""; 
  Serial.println("Reading response...");
  int i = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      response += c;
      i++;
      do {
          char c = client.read();
          response += c;
          i++;
      } while(client.available());
    } 
  }
  Serial.println("Read " + String(i, DEC) + " chars.");
  client.stop();
  Serial.println("disconnecting...");
  return response;
}

String processResponse(String r){
  String line = "";
  String event = "";
  int found = 0;
  int len = r.length();
  
  Serial.println("Processing response..." + String(len, DEC) );
  
  for (int i=0; i<=len; i++){
    char c = r[i];
    line += c;
    if ( c == '\n'  || i == len){
      Serial.print("line: " + line);
      // new line
      int eventsPos = line.indexOf("eventid");
      if (eventsPos == -1){
        line = String("");
      }
      else{
        int m = line.length();
        found = 1;
        Serial.println("Found at " + String(eventsPos, DEC) + ", " + String(m, DEC));
        int j = 0;
        boolean f = false;
        while (j < m && !f ){
          char c = line[j];
          if (c == ';'){
            f = true;
            event = line.substring(0, j);
          }
          j++;
        }
        if ( !f ){
          event = line.substring(0, m);
        }
      }  
    }
  }
  if ( found != 1){
    Serial.println("Found no events to process on response.");
    event = "";
  }
  Serial.println("Returning event " + event);
  return event; 
}

void processEvent(String eventid, String productid, String dest, String type){
  Serial.println("Processing event... ");
  if (dest != moduleId){
    Serial.println("Ignoring event for dest " + dest);
    return;
  }  
  // activate relee
  String st = vend(productid);
  // confirm
  while (confirmEvent(eventid, st) == 0){
    delay(5000);
  }
}

String vend(String productid){
  String st = "ng";
  int relay = getIntValue(productid);
  Serial.println("Vend!!! " + String(relay, DEC));
  if (relay >= 1 && relay <=8){
    digitalWrite(VEND_RELAY, HIGH);
    delay(100);
    digitalWrite(VEND_RELAY, LOW);
    digitalWrite(relay, HIGH);
    delay(100);
    digitalWrite(relay, LOW);
    st = "ok";
  }
  delay(1000);
  
  return st;
}

int confirmEvent(String eventid, String st){
  int confirmed = 0;
    Serial.println("Confirm event... ");
  if (client.connect(server, 9090)) {
    Serial.println("Sending event confirmation...");
    client.println("GET /app/events?a=u&id=" + moduleId + "&e=" + eventid + "&r=" + st + " HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
    confirmed = 1;
    client.stop();
  }
  else{
    Serial.println("Sending event confirmation failed");
  }
  return confirmed;
}

void processEventString (String data){
  int pos = 0;
  String eventid;
  String productid;
  String dest;
  String type;  

  String thisToken;
  
  Serial.println("Processing event string: " + data);
  do
  {
      pos = data.indexOf(',');
      if(pos != -1)
      {
        thisToken = data.substring(0, pos);
        data = data.substring( pos+1, data.length());
        // printKeyValue(thisToken);      
      }
      else
      {  // here after the last comma is found
         if(data.length() > 0){
           thisToken = data;
           //Serial.println("2: " + thisToken); 
           //printKeyValue(thisToken);
         }
      }
      String thisKey = getKey(thisToken);
      String value = getValue(thisToken);
      if (thisKey == "eventid"){
        eventid = value ;
      }
      else if (thisKey == "productid"){
        productid = value;
      }
      else if (thisKey == "dest"){
        dest = value;
      }
      else if (thisKey == "type"){
        type = value ;
      }
   }
   while(pos >=0);
Serial.println("Finish process event string: ");
   processEvent(eventid, productid, dest, type);
}

String getKey (String token){
  int pos = 0;

  pos = token.indexOf('=');
  return token.substring(0, pos);  
}

String getValue(String data){
  int pos = 0;

  pos = data.indexOf('=');
  return data.substring( pos+1, data.length());
}

void printKeyValue(String data){
  int pos = 0;

  pos = data.indexOf('=');
  String thisKey = data.substring(0, pos);  
  String value = data.substring( pos+1, data.length());
  Serial.println("[" + thisKey + "," + value + "]"); 
  
}

int getIntValue(String value){
  int len = value.length() +1; 
  char num[len];
  value.toCharArray(num, len); 
  return atoi(num);
}


