#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "TP-3009";
const char* password = "30092016vm";

// Pin connected to relay
const int relayPin = 16;

AsyncWebServer server(80);

void setup() {
  pinMode(relayPin, OUTPUT);
  
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><h1>ESP32 Relay Control</h1><p>Click <a href='/relay/on'>here</a> to turn the relay ON.</p><p>Click <a href='/relay/off'>here</a> to turn the relay OFF.</p></body></html>");
  });

  server.on("/relay/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(relayPin, HIGH);
    request->send(200, "text/plain", "Relay turned ON");
  });

  server.on("/relay/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(relayPin, LOW);
    request->send(200, "text/plain", "Relay turned OFF");
  });

  server.begin();
}

void loop() {

}
