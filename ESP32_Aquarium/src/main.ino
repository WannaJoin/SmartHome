#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHTesp.h>
#include <ESP32Time.h>

using namespace websockets;

#define LIGHT_PIN 16
#define BUTTON_PIN 21
#define DHT_PIN 16
#define DHT_TYPE DHT11

const char* ssid = "TP-3009";
const char* password = "30092016vm";

const char* wsServer = "192.168.10.214"; 
const uint16_t wsPort = 8080; 
WebsocketsClient wsClient;

//Sensors
const int oneWireBus = 4; 
OneWire oneWire(oneWireBus);
DallasTemperature waterSensor(&oneWire);
DHTesp dht;
/////////////////////////////

bool wsConnected;
bool lightState = false;
TaskHandle_t ButtonTask;
TaskHandle_t ClockTask;
ESP32Time rtc(-3600);

String parsedData(){
  waterSensor.requestTemperatures();
  delay(dht.getMinimumSamplingPeriod());
  
  delay(20000);
  dht.getHumidity();
  dht.getTemperature();

  JsonDocument sensorsData;
  sensorsData["waterTemp"] = waterSensor.getTempCByIndex(0);
  sensorsData["airTemp"] = dht.getTemperature();
  sensorsData["humidity"] = dht.getHumidity();
  String message;
  serializeJson(sensorsData, message);
  return message;
}

void inMessageHandler(String message){
}

void wsConnect(){
  wsConnected = wsClient.connect(wsServer, wsPort, "/");

  if(wsConnected) {
    Serial.println("Connecetd!n");
  } else {
    Serial.println("Not Connected!");
  }
}

void checkButton(void *parameter){
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  while(true){
    if(digitalRead(BUTTON_PIN) == LOW){
      Serial.println("Button was pressed!");
      if(lightState) lightState = false;
      else lightState = true;
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void checkHour(void *parameter){
  while (true) {
    int currentHour = rtc.getHour(true);
    Serial.print("Current Hour: ");
    Serial.println(currentHour);

    if (currentHour >= 8 && currentHour <= 21) {
      Serial.println("8AM: Turn Ligth On");
      lightState = true;
    } else {
      Serial.println("9PM: Turn Light Off");
      lightState = false;
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}

void setup(){
  xTaskCreate(checkButton, "checkButton", 10000, NULL, 1, &ButtonTask);
  xTaskCreate(checkHour, "checkHour", 10000, NULL, 1, &ClockTask);

  Serial.begin(115200);
  delay(100);

  pinMode(LIGHT_PIN, OUTPUT);
  dht.setup(DHT_PIN, DHTesp::DHT11);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Connected to WiFi");
  Serial.print("IP: ");
  Serial. println(WiFi. localIP());
  
  wsConnect();
  wsClient.onMessage([&](WebsocketsMessage message) {
      Serial.print("Got Message: ");
      Serial.println(message.data());
      inMessageHandler(message.data());
    }
  );
  delay(3000);
}

void loop(){  
  Serial.println("Serial: ");
  Serial.println(parsedData());

  if(wsClient.available()) {
    wsClient.poll();
    wsClient.send(parsedData());
  } else {
    wsConnect();
  }

  if (lightState){
    digitalWrite(LIGHT_PIN, HIGH);
  } else {
    digitalWrite(LIGHT_PIN, LOW);
  }
  delay(500);
}