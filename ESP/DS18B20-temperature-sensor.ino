#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>


const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80); //Server on port 80
IPAddress local_IP(192, 168, 1, 252);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char* ssid = "YOURNETWORKNAME";
const char* password = "YOURNETWORKPASSWORD";
unsigned long delayTimeInMillis = 2000;
unsigned long millisSincePreviousCheck = 0;
unsigned long currentMillis;
float temperature;

void setup() {
    connectToWifi();
    configureRoutes();
    sensors.begin();
    server.begin();
    delay(delayTimeInMillis);
}

void connectToWifi(){
    WiFi.config(local_IP, gateway, subnet);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void configureRoutes(){
    server.on("/", handleRoot);
}

void handleRoot() {
  server.send(200, "application/text", 
    "temperature " + String(temperature) +
    "\n\n"
  );
}


void loop(void){
  currentMillis = millis();
  if(currentMillis < delayTimeInMillis){
    millisSincePreviousCheck = 0;
  }
  if(currentMillis - millisSincePreviousCheck > delayTimeInMillis){
      sensors.requestTemperatures(); 
      millisSincePreviousCheck = currentMillis;
      temperature = sensors.getTempCByIndex(0);
  }
  server.handleClient();
}