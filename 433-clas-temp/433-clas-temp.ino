/*
 Basic ESPilight receive example

 https://github.com/puuu/espilight
*/

#include <ESPiLight.h>


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ArduinoJson.h>
#define RECEIVER_PIN 4  // any intterupt able pin
#define TRANSMITTER_PIN 13

ESPiLight rf(TRANSMITTER_PIN);  // use -1 to disable transmitter

ESP8266WiFiMulti WiFiMulti;


//setup urls
const char* host = "api.thingspeak.com";
//String apikey = "GHDP2ARMIAP37BEI"; //longterm
String apikey = "X8VCD0R63EWKXSQH";
String url = "/update.html?key="+apikey+"&field1=";
String payload ="";

// callback function. It is called on successfully received and parsed rc signal
void rfCallback(const String &protocol, const String &message, int status,
                size_t repeats, const String &deviceID) {
  Serial.print("RF signal arrived [");
  Serial.print(protocol);  // protocoll used to parse
  Serial.print("][");
  Serial.print(deviceID);  // value of id key in json message
  Serial.print("] (");
  Serial.print(status);  // status of message, depending on repeat, either:
                         // FIRST   - first message of this protocoll within the
                         //           last 0.5 s
                         // INVALID - message repeat is not equal to the
                         //           previous message
                         // VALID   - message is equal to the previous message
                         // KNOWN   - repeat of a already valid message
  Serial.print(") ");
  Serial.print(message);  // message in json format
  Serial.println();

  // check if message is valid and process it
  if (status == VALID) {
    Serial.print("Valid message: [");
    Serial.print(protocol);
    Serial.print("] ");
    Serial.print(message);
    Serial.println();

    //parse message
     StaticJsonBuffer<200> jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(message);
     delay(100);
     String validtemp = root["temperature"];
     String tempchannel = root["channel"];
     Serial.print("Got new payload:"+validtemp);

Serial.print("connecting to ");
  Serial.println(host);

  if(tempchannel=="2"){
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(url + validtemp);
  
  // This will send the request to the server
  client.print(String("GET ") + url + validtemp + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  }
    
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting wifi..");

 // We start by connecting to a WiFi network
    WiFiMulti.addAP("capcom", "stygtpent");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
  
  // set callback funktion
  rf.setCallback(rfCallback);
  // inittilize receiver
  rf.initReceiver(RECEIVER_PIN);
}

void loop() {
  // process input queue and may fire calllback
  rf.loop();
  delay(10);
}
