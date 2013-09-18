
#include <SPI.h>
#include <Ethernet.h>

#define VEND_RELAY 14               // PIN for enabling vend relay
#define RELAY_1 2                  // PIN for enabling column 1 relay
#define RELAY_2 3                  // PIN for enabling column 2 relay
#define RELAY_3 15                  // PIN for enabling column 3 relay
#define RELAY_4 5                  // PIN for enabling column 4 relay
#define RELAY_5 6                  // PIN for enabling column 5 relay
#define RELAY_6 7                  // PIN for enabling column 6 relay
#define RELAY_7 8                  // PIN for enabling column 7 relay
#define RELAY_8 9                  // PIN for enabling column 8 relay

#define QUERY_TIME 6000           // time between event queries.

#define RELAY_ON_TIME 1000          // milisecs of time the relay will be enabled.

#define SERVER_PORT 9080           // app server port

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte gateway[] = {192, 168, 1, 1};
//IPAddress server(192,168,1,178);  // app server
// windsolvendmodule-env.elasticbeanstalk.com
//char server[] = "windsolvendmodule-env.elasticbeanstalk.com";
char server[] = "synt.dyndns.org";

// Set the static IP address to use.
IPAddress ip(192,168,1,90);

// ID of the Vending Machine where this module is installed
String moduleId  = "1";

unsigned long time;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup()
{
  digitalWrite(RELAY_1, 1);
  digitalWrite(RELAY_2, 1);
  digitalWrite(RELAY_8, 1);
  digitalWrite(RELAY_4, 1);
  digitalWrite(RELAY_5, 1);
  digitalWrite(RELAY_6, 1);
  digitalWrite(RELAY_7, 1);
  digitalWrite(RELAY_3, 1);
  digitalWrite(VEND_RELAY, 1);  

  delay(1000);
  
  // Set all relay pins as OUTPUT
  // tells arduino RELAY is an output
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_8, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  pinMode(RELAY_5, OUTPUT);
  pinMode(RELAY_6, OUTPUT);
  pinMode(RELAY_7, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(VEND_RELAY, OUTPUT);
  
  delay(1000);    
  // Open serial communications:
  Serial.begin(9600);

  // start the Ethernet connection:
  Serial.println("try to configure using dynamic IP address");
  int result = 0;
  while ( result != 1) {
    result = Ethernet.begin(mac);
    delay(100);
  }
  Serial.println("took dynamic ip");
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
      time = millis();
      client.println("GET /events/events?a=q&id=" + moduleId + "&" + String(time, DEC) + " HTTP/1.1");
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
  int loops = 0;
  while (client.connected() && !found && loops < 1000) {
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
          loops = 0;
      } while(client.available() && !found);
    }
    else {
      loops++;
      delay(10);
    }  
  }
  if (loops >= 1000){
    Serial.println("Reading timeout...");
  }
  Serial.println("Read " + String(i, DEC) + " chars. Returned " + String(response.length(), DEC));
  client.flush();
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
  int st = vend(productid);
  //Serial.println("After vend " + st );
  // confirm
  int retries = 3;
  while (confirmEvent(eventid, st) == 0 && retries > 0){
    retries--;
    delay(5000);
  }
}

/*
* Activates the vend relay and then activates the corresponding selection panel relay.
*/
int vend(String productid){
  int vend = 0;
  int p = getIntValue(productid);
  int relay = getRelay(p);
  Serial.println("Vend!!! " + productid);
  if (p >= 1 && p <=8){
    digitalWrite(VEND_RELAY, LOW);
    delay(100);
    digitalWrite(VEND_RELAY, HIGH);
    Serial.println("Vend enable. Now opening relay " + String(relay, DEC));
    delay(500);
    
    digitalWrite(relay, LOW);
    delay(RELAY_ON_TIME);
    digitalWrite(relay, HIGH);
   
    delay(500);
    Serial.println("Vend done ");
    vend = 1;
  }
  delay(2000);
  //Serial.println("Vend done " + productid + " " + st);
  return vend;
}

/*
 * Sends event confirmation to the server.
 */
int confirmEvent(String eventid, int s){
  int confirmed = 0;
  String st = (s == 1) ? "ok" : "ng";
  Serial.println("Confirm event... ");
  client.stop();
  if (client.connect(server, SERVER_PORT)) {
    Serial.println("Sending event confirmation [" + eventid + "]");
    client.println("GET /events/events?a=u&id=" + moduleId + "&e=" + eventid + "&r=" + st + " HTTP/1.1");
    client.println("Host: arduino");
    client.println("Connection: close");
    client.println();
    confirmed = 1;
    //client.flush();
    client.stop();
    Serial.println("Event [" + eventid + "] confirmed.");
  }
  else{
    client.stop();
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

int getRelay( int p ){
  if ( p == 1){
    return RELAY_1;
  }
  else if ( p == 2){
    return RELAY_2;
  } 
  else if ( p == 3){
    return RELAY_3;
  } 
  else if ( p == 4){
    return RELAY_4;
  } 
  else if ( p == 5){
    return RELAY_5;
  } 
  else if ( p == 6){
    return RELAY_6;
  } 
  else if ( p == 7){
    return RELAY_7;
  } 
  else if ( p == 8){
    return RELAY_8;
  } 
  return RELAY_1;  
}


