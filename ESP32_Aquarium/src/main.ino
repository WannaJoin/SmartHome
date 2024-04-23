#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LIGHT_PIN 16
#define HEAT_PIN 17 

const char* ssid = "TP-3009";
const char* password = "30092016vm";

const char* wsServer = "192.168.10.246"; 
const uint16_t wsPort = 8080; 

using namespace websockets;
WebsocketsClient wsClient;

const int oneWireBus = 4; 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

bool wsConnected;
float sensorValue = 0.1;

bool lightState = false;
bool heatState = false;



void setup(){
  Serial.begin(115200);
  delay(100);

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
  });

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  delay(3000);
}

void loop(){
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
  delay(900000);
}

void inMessageHandler(String message){
    if (message == "l"){
      if(lightState){
        lightState = false;
        digitalWrite(LIGHT_PIN, LOW);
        wsClient.send("loff");
      } else {
        lightState = true;
        digitalWrite(LIGHT_PIN, HIGH);
        wsClient.send("lon");
      }
    }

    if (message == "h"){
      if(heatState){
        heatState = false;
        digitalWrite(HEAT_PIN, LOW);
        wsClient.send("hoff");
      } else {
        heatState = true;
        digitalWrite(HEAT_PIN, HIGH);
        wsClient.send("hon");
      }
    }
}

void wsConnect(){
  wsConnected = wsClient.connect(wsServer, wsPort, "/");

  if(wsConnected) {
    Serial.println("Connecetd!n");
  } else {
    Serial.println("Not Connected!");
  }
}