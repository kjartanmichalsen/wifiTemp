#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D4);


// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//init wifimanager
WiFiManager wifiManager;

//setup urls
const char* host = "api.thingspeak.com";
String apikey = "GHDP2ARMIAP37BEI";
String url = "/update.html?key="+apikey+"&field1=";
String payload ="";

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Get first temperature");

  // Start up the library
  sensors.begin();
  sensors.requestTemperatures(); // Send the command to get temperatures  

  Serial.println("Try to connect to WIFI...");
  wifiManager.autoConnect("Netverdata");
}


 
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
 Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  payload = String(sensors.getTempCByIndex(0),2);
  
  Serial.println("DONE");

  Serial.print("Temperature for Device 1 is: ");
  Serial.print(payload); 
  
 delay(1000);
 
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + payload + " HTTP/1.1\r\n" +
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
 ESP.deepSleep(1800000000);
}



