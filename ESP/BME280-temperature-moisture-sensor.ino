#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

// assign the ESP8266 pins to arduino pins
#define D1 5
#define D2 4
#define D4 2
#define D3 0

// assign the SPI bus to pins
#define BME_SCK D1
#define BME_MISO D4
#define BME_MOSI D2
#define BME_CS D3

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

ESP8266WebServer server(80); //Server on port 80
IPAddress local_IP(192, 168, 1, 254);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const char* ssid = "YOURNETWORKNAME";
const char* password = "YOURNETWORKPASSWORD";
unsigned long delayTimeInMillis = 2000;
unsigned long millisSincePreviousCheck = 0;
unsigned long currentMillis;

float humidity;
float temperature;
float pressure;

void setup() {
    connectToWifi();
    configureRoutes();
    
    bme.begin();
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
    "humidity " + String(humidity) +
    "\ntemperature " + String(temperature) +
    "\npressure " + String(pressure) +
    "\n\n"
  );
}


void loop(void){
  currentMillis = millis();
  if(currentMillis < delayTimeInMillis){
    millisSincePreviousCheck = 0;
  }
  if(currentMillis - millisSincePreviousCheck > delayTimeInMillis){
      millisSincePreviousCheck = currentMillis;
      humidity = bme.readHumidity();
      temperature = bme.readTemperature();
      pressure = bme.readPressure() / 100.0F;
  }
  server.handleClient();
}