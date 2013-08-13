
#include <SPI.h>
#include <Ethernet.h>

#define VEND_RELAY 9               // PIN for enabling vend relay
#define RELAY_1 1                  // PIN for enabling column 1 relay
#define RELAY_2 2                  // PIN for enabling column 2 relay
#define RELAY_3 3                  // PIN for enabling column 3 relay
#define RELAY_4 4                  // PIN for enabling column 4 relay
#define RELAY_5 5                  // PIN for enabling column 5 relay
#define RELAY_6 6                  // PIN for enabling column 6 relay
#define RELAY_7 7                  // PIN for enabling column 7 relay
#define RELAY_8 8                  // PIN for enabling column 8 relay

#define QUERY_TIME 10000           // time between event queries.

#define RELAY_ON_TIME 100          // milisecs of time the relay will be enabled.

#define SERVER_PORT 9090           // app server port

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
  delay(QUERY_TIME);
}

/*
 * Queries the application server for new events to consume.
 */
int query(){
   int queried = 0;
   Serial.println("Querying...");

   if (client.connect(server, SERVER_PORT)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /app/events?a=q&id=" + moduleId + " HTTP/1.1");
      client.println("Host: arduino");
      client.println("Connection: close");
      client.println();
      queried = 1;
    } 
    else {
      Serial.println("connection failed");
    } 
    return queried;
}

/*
* Reads server response and returns it for processing.
*/
String getResponse(){
  String response = "";
  boolean found = false; 
  Serial.println("Reading response...");
  int i = 0;
  while (client.connected() && !found) {
    delay(1);
    if (client.available() && !found) {
      do {
          char c = client.read();
          // Serial.print("["  + String(i, DEC) + "]:" + String(c, HEX) + " ,");
          response += c;
          if ( c == '\n' && response.indexOf("eventid") == -1){
            response = String("");
          }
          if ( c == ';' && response.indexOf("eventid") >= 0){
            // found it
            found = true;
          } 
          i++;
      } while(client.available() && !found);
    } 
  }
  Serial.println("Read " + String(i, DEC) + " chars. Returned " + String(response.length(), DEC));
  client.stop();
  Serial.println("disconnecting...");
  return response;
}

/*
 * Processes response string and splits it into event strings in
 * case there are more than one event.
 */
String processResponse(String r){
  String event = "";
  Serial.println("Processing response...");  
  int eventsPos = r.indexOf(";");
  if (eventsPos == -1){
    event = r.substring(0, r.length());
  }
  else{
    event = r.substring(0, eventsPos);
  }
  Serial.println("Returning event " + event);
  return event; 
}

/*
 * Processes an event. It activates the vend and sends event confirmation if all wents ok.
 */
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

/*
* Activates the vend relay and then activates the corresponding selection panel relay.
*/
String vend(String productid){
  String st = "ng";
  int relay = getIntValue(productid);
  Serial.println("Vend!!! " + String(relay, DEC));
  if (relay >= 1 && relay <=8){
    digitalWrite(VEND_RELAY, HIGH);
    delay(RELAY_ON_TIME);
    digitalWrite(VEND_RELAY, LOW);
    digitalWrite(relay, HIGH);
    delay(RELAY_ON_TIME);
    digitalWrite(relay, LOW);
    st = "ok";
  }
  delay(1000);
  
  return st;
}

/*
 * Sends event confirmation to the server.
 */
int confirmEvent(String eventid, String st){
  int confirmed = 0;
  Serial.println("Confirm event... ");
  if (client.connect(server, SERVER_PORT)) {
    Serial.println("Sending event confirmation [" + eventid + "]");
    client.println("GET /app/events?a=u&id=" + moduleId + "&e=" + eventid + "&r=" + st + " HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
    confirmed = 1;
    client.stop();
    Serial.println("Event [" + eventid + "] confirmed.");
  }
  else{
    Serial.println("Event [" + eventid + "] confirmation failed");
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
      }
      else
      {  // here after the last comma is found
         if(data.length() > 0){
           thisToken = data;
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

int getIntValue(String value){
  int len = value.length() +1; 
  char num[len];
  value.toCharArray(num, len); 
  return atoi(num);
}


