#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "TP-3009";
const char* password = "30092016vm";
const char* serverUrl = "http://192.168.10.246:6161/setWaterVal";

const int oneWireBus = 4; 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float sensorValue = 0.1; // This is just an example float value

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "value=" + String(temperatureC);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      Serial.print("Data sent: ");
      Serial.println(postData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }


  esp_sleep_enable_timer_wakeup(300 * 1000000); // 30 seconds
  esp_deep_sleep_start();
}
