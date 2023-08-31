#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define SEALEVELPRESSURE_HPA (1013.25)
const int buttonPin = 0;

Adafruit_BME280 bme;
HTTPClient http;
WiFiClient client;
int counter = 0;


void setup()
{
 Serial.begin(115200);
 

 Serial.println(F("BME280 test"));
 bool status;

 status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  } 
    WiFi.begin("Greenscreens.ai", "Hs-AT14<=:84aKx6");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

}

void loop()
{
  printValues();
  delay(10000);
}


void sendRequest(float t, float h, float p) {
  Serial.print("[HTTP] begin...\n");
  counter += 1;
  String url = "http://192.168.0.244/request?temperature=" + String(t) + "&humidity=" + String(h) + "&pressure=" + String(p) + "&counter=" + counter;
    if (http.begin(client, url)) {
      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    }
}


void printValues() {
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F; 
  float humidity = bme.readHumidity(); 
  sendRequest(temperature, humidity, pressure);
}