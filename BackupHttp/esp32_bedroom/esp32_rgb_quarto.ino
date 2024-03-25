#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define RED_PIN   12  
#define GREEN_PIN 14
#define BLUE_PIN  13  
#define NUM_PIXELS 60

const char* ssid = "TP-3009";
const char* password = "30092016vm";

String rgbArr[] = {"0","0","0"};
String bRgbArr[] = {"0","0","0"};
String payload = "";
HTTPClient http;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to WiFi");

  xTaskCreatePinnedToCore(httpServerRequest, "HTTP Request Task", 10000, NULL, 1, NULL, 0);
}

void loop() {
  
  if (rgbArr[0] == "fire"){
    fireEffect();
  } else if (rgbArr[0] == "flash"){
    flashEffect();
  } else if (rgbArr[0] == "on"){
    onHandler();
  } else if (rgbArr[0] == "off"){
    offHandler();
  } else {
    rgbChecker(rgbArr[0].toInt(),rgbArr[1].toInt(), rgbArr[2].toInt());
  }

  saveBackArr();

  delay(50);
}


void saveBackArr(){
  if (rgbArr[0] != "on" && rgbArr[0] != "off" && rgbArr[0] != "more" && rgbArr[0] != "less" 
  && (rgbArr[0] != "0" || rgbArr[1] != "0" || rgbArr[2] != "0")){
    
    bRgbArr[0] = rgbArr[0];
    bRgbArr[1] = rgbArr[1];
    bRgbArr[2] = rgbArr[2];
  }
}

void onHandler(){
  rgbArr[0] = bRgbArr[0];
  rgbArr[1] = bRgbArr[1];
  rgbArr[2] = bRgbArr[2];
}

void offHandler(){
  rgbArr[0] = "0";
  rgbArr[1] = "0";
  rgbArr[2] = "0";
}

void httpServerRequest (void * parameter){
  for (;;) {
    http.begin("http://192.168.10.246:3001/reqrgb");
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.print("Response from server:  [ ");

      DynamicJsonDocument doc(1024); 
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());
      } else {
        if (doc.is<JsonArray>()) {
          JsonArray svArr = doc.as<JsonArray>();
          for (size_t i = 0; i < svArr.size(); i++) {
            rgbArr[i] = svArr[i].as<String>();
            Serial.print(rgbArr[i]);
            if (i != 2) {Serial.print(", ");}
          }
          Serial.println(" ]");
        } else {
          Serial.println("Received data is not an array");
        }
      }
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void rgbChecker(int r, int g, int b){
  if (r >= 0 && r <= 255 && 
  g >= 0 && g <= 255 &&
  b >= 0 && b <= 255){
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);  
    analogWrite(BLUE_PIN, b);
  } else {
    Serial.println("RGB CODE INVALID!");
  }
}

void flashEffect(){ 
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);  
  analogWrite(BLUE_PIN, 0);
  
  auto fadeInOut = [](int pin) {
    int fadeDelay = 2;

    for (int i = 0; i <= 255; i++) {
      analogWrite(pin, i);
      delay(fadeDelay);
    }
    for (int i = 255; i >= 0; i--) {
      analogWrite(pin, i);
      delay(fadeDelay);
    }
  };
  fadeInOut(RED_PIN);
  fadeInOut(GREEN_PIN);
  fadeInOut(BLUE_PIN);
}

void fireEffect() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    int flicker = random(125, 255);
    analogWrite(RED_PIN, flicker);
    analogWrite(GREEN_PIN, flicker / 50);
    analogWrite(BLUE_PIN, 0);
    delay(10);
  }
}
