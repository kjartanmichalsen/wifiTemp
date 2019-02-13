// Setup WIFI with 192.168.4.1

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

#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <SoftwareSerial.h>
#include "MHZ.h"

// pin for uart reading
#define CO2_IN D3

// pin for pwm reading
#define MH_Z19_RX D7  // D7
#define MH_Z19_TX D8  // D6

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);

//init wifimanager
WiFiManager wifiManager;

//setup urls
const char* host = "api.thingspeak.com";
String apikey = "OF1BJKT912IEISTF";
String url = "/update.html?key="+apikey+"&field1=";
String cotwo ="";
String temp ="";

void setup(void)
{
  Serial.begin(9600);
  pinMode(CO2_IN, INPUT);
  delay(100);
  Serial.println("MHZ 19B");

  // enable debug to get addition information
  // co2.setDebug(true);

  if (co2.isPreHeating()) {
    Serial.print("Preheating");
    while (co2.isPreHeating()) {
      Serial.print(".");
      delay(5000);
    }
    Serial.println();
  }

  Serial.println("Try to connect to WIFI...");
  wifiManager.autoConnect("temperatureLogger");
}


 
void loop(void)
{

  Serial.print("\n----- Time from start: ");
  Serial.print(millis() / 1000);
  Serial.println(" s");

  int ppm_uart = co2.readCO2UART();
  if (ppm_uart > 0) {
    Serial.print("PPMuart: ");
    Serial.print(ppm_uart);
  }

  int ppm_pwm = co2.readCO2PWM();
  Serial.print(", PPMpwm: ");
  Serial.print(ppm_pwm);

  int temperature = co2.getLastTemperature();
  if (temperature > 0) {
    Serial.print(", Temperature: ");
    Serial.println(temperature);
  }

  cotwo = ppm_pwm;
  temp = temperature;

  Serial.println("\n------------------------------");
  delay(5000);
 
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
  client.print(String("GET ") + url + cotwo + "&field2=" + temp + " HTTP/1.1\r\n" +
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
 //ESP.deepSleep(1800000000);
 delay(300000);
}



