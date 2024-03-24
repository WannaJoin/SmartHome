#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#define TIPPIN 12
#define BUZZPIN 26

AsyncWebServer server(80);

const char* ssid = "MEO-741A60";
const char* password = "1b80343787";

const int numAttemps = 10;

const char* hotSsid = "ESP32";
const char* hotPassword = "12345678";

const char * htmlCode = R"(
  <html>
  <head>
    <title>ESP32 interface</title>
    <style>
      #powerButton{
        width: 70px; 
        height: 60px;
        font-size: large; 
        border-radius: 12px;
        color: black;
        background-color: #5adbb5;
        font-weight: bold;
      }
    </style>
  </head>
    <body>
      <a href="/power">
        <button id="button">Power</button>
      </a>
      <a href="/off">
        <button id="button">Off</button>
      </a>
      <script>
        document.getElementById("button").addEventListener("click", function() {
          var h1 = document.createElement("h1");
          var text = document.createTextNode("Command sent");
          h1.appendChild(text);
          document.body.appendChild(h1);
        });
      </script>
    </body>
  </html>
  )";

void setup() {
  pinMode(TIPPIN, OUTPUT);
  
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int i = 0;
  bool hotSpot = false;

  while (WiFi.status() != WL_CONNECTED && i < numAttemps) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    i++;
  }

  if (WiFi.status() == WL_CONNECTED){
    Serial.print("Connected to: ");
    Serial.println(ssid);
    Serial.println(WiFi.localIP());
    tone(BUZZPIN, 1, 1000);
  } else {
    Serial.print("Failed to connect to: ");
    Serial.println(ssid);
    Serial.println("Starting the hotspot.");
    hotSpot = true;
  }

  if (hotSpot){
    WiFi.softAP(hotSsid, hotPassword);
    Serial.println("Access Point started");
    delay(1000);

    Serial.println(WiFi.softAPIP());
    
    tone(BUZZPIN, 1, 200);
    delay(400);
    tone(BUZZPIN, 1, 200);
  }
  

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("/");
    request->send(200, "text/html", htmlCode);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
      analogWrite(TIPPIN, 255);
      delay(5000);
      analogWrite(TIPPIN, 0);
      Serial.println("/power");
      request->send(200, "text/html", htmlCode);
  });


  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
    analogWrite(TIPPIN, 255);
    delay(500);
    analogWrite(TIPPIN, 0);
    Serial.println("/power");
    request->send(200, "text/html", htmlCode);
  });

  server.begin();
}

void loop() {

}