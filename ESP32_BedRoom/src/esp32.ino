#include "DHT.h"
#include "TM1637Display.h"


#define DHTPIN 4
//Wokwi uses DHT22 but at home I use DHT11
//#define DHTTYPE DHT11
#define DHTTYPE DHT22

#define CLKT  26
#define DIOT  27

#define CLKH  14
#define DIOH  12

TM1637Display displayT = TM1637Display(CLKT, DIOT);
TM1637Display displayH = TM1637Display(CLKH, DIOH);

DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);

  dht.begin();

  displayT.clear();
  displayH.clear();
  displayT.setBrightness(7);
  displayH.setBrightness(7);
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
  Serial.println(F("% "));
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));
  Serial.println("");

  displayT.showNumberDecEx((int)(hic * 100), 0b00100000, false, 4, 0);
  displayH.showNumberDecEx((int)(h * 100), 0b00100000, false, 4, 0);
}