#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "TP-3009";
const char* password = "x";

const char* wsServer = "192.168.10.x"; 
const uint16_t wsPort = 8080; 

using namespace websockets;
WebsocketsClient wsClient;

const int oneWireBus = 4; 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

bool wsConnected;
float sensorValue = 0.1;

void wsConnect(){
  wsConnected = wsClient.connect(wsServer, wsPort, "/");

  if(wsConnected) {
    Serial.println("Connecetd!n");
  } else {
    Serial.println("Not Connected!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP: ");
  Serial. println(WiFi. localIP());
  
  wsConnect();
  wsClient.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
    });
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  
  if(wsClient.available()) {
    wsClient.poll();
    wsClient.send(String(temperatureC));
  } else {
    wsConnect();
  }
  delay(2000);
}


