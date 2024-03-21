#include "DHT.h"
#include "TM1637Display.h"


#define DHTPIN 4
#define DHTTYPE DHT11

#define CLKT  26
#define DIOT  27

TM1637Display displayT = TM1637Display(CLKT, DIOT);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  displayT.clear();
  displayT.setBrightness(7);
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(t);
  Serial.println(F("% "));
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));
  Serial.println("");

  displayT.showNumberDecEx((int)(hic * 100), 0b00100000, false, 4, 0);
}