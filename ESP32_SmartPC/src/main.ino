#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define TIPPIN 12
#define BUZZPIN 26

AsyncWebServer server(80);

const char* ssid = "Convidados";
const char* password = "";

const int numAttemps = 10;

const char* hotSsid = "ESP32";
const char* hotPassword = "12345678";

const char * htmlCode = R"(
  <html>
    <head>
      <title>ESP32 interface</title>
      
      <style>
        .button{
          width: 70px; 
          height: 60px;
          font-size: large; 
          border-radius: 12px;
          color: black;
          
          font-weight: bold;
        }

        #bPower{background-color: #5adbb5;}
        #bOff{background-color: #ffbd03;}
      </style>
    
  </head>
      <body>
          <br>
          <button onclick='powerHandler()' class='button' id='bPower'>
              <svg width='50px' height='50px' viewBox='0 0 24 24' fill='none' xmlns='http://www.w3.org/2000/svg'>
                  <path d='M16.6582 9.28638C18.098 10.1862 18.8178 10.6361 19.0647 11.2122C19.2803 11.7152 19.2803 12.2847 19.0647 12.7878C18.8178 13.3638 18.098 13.8137 
                  16.6582 14.7136L9.896 18.94C8.29805 19.9387 7.49907 20.4381 6.83973 20.385C6.26501 20.3388 
                  5.73818 20.0469 5.3944 19.584C5 19.053 5 18.1108 5 16.2264V7.77357C5 5.88919 5 4.94701 
                  5.3944 4.41598C5.73818 3.9531 6.26501 3.66111 6.83973 3.6149C7.49907 3.5619 8.29805 4.06126 9.896 
                  5.05998L16.6582 9.28638Z' stroke='#000000' stroke-width='2' stroke-linejoin='round'/>
              </svg>
          </button>
          <br><br>
          <button onclick='offHandler()' class='button' id='bOff'>
              <svg width='60px' height='60px' viewBox='-0.5 0 25 25' fill='none' xmlns='http://www.w3.org/2000/svg'>
                  <path id='Vector' d='M5 8.2002V15.8002C5 16.9203 5 17.4796 5.21799 17.9074C5.40973 18.2837 5.71547 18.5905 6.0918 18.7822C6.5192 
                  19 7.07899 19 8.19691 19H15.8036C16.9215 19 17.4805 19 17.9079 18.7822C18.2842 18.5905 18.5905 18.2837 18.7822 17.9074C19 17.48 19 
                  16.921 19 15.8031V8.19691C19 7.07899 19 6.5192 18.7822 6.0918C18.5905 5.71547 18.2842 5.40973 17.9079 5.21799C17.4801 5 16.9203 5 
                  15.8002 5H8.2002C7.08009 5 6.51962 5 6.0918 5.21799C5.71547 5.40973 5.40973 5.71547 5.21799 6.0918C5 6.51962 5 7.08009 5 8.2002Z' 
                  stroke='#000000' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'/>
              </svg>
          </button>
          <br><br><br>
          <h2 id='message'></h1>

          <script>
              function powerHandler(){
                  bGeneral();
                  setTimeout(() => window.location.href = '/power', 2000);
              }

              function offHandler(){
                  if (window.confirm("Are you sure that you want to force your computer to shutdown?")){
                      bGeneral();
                      setTimeout(() => window.location.href = '/off', 2000);
                  } else {
                      window.alert("That was close!");
                  }
              }

              function bGeneral(){
                  document.getElementById('message').innerHTML = "Command sent, please stand by";
                  document.getElementById('bPower').disabled = true;
                  document.getElementById('bOff').disabled = true;
              }
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
    Serial.println("Website loaded on client device");
    request->send(200, "text/html", htmlCode);
  });


  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("/power");
    
    analogWrite(TIPPIN, 255);
    delay(500);
    analogWrite(TIPPIN, 0);
    
    request->redirect("/");
  });


  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("/off");
      
      analogWrite(TIPPIN, 255);
      delay(5000);
      analogWrite(TIPPIN, 0);
      
      request->redirect("/");
  });


  server.begin();

}

void loop() {
  
}

