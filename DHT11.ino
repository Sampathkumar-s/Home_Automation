#define BLYNK_TEMPLATE_ID "TMPL3sGPsPOd5"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "iaqNBhGf1jNjQAio54d49ZKPmALQohDZ";
char ssid[] = "POCO C55";
char pass[] = "sampathkumar";

#define DHTPIN D4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendDHT() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ DHT read failed");
    return;
  }

  // Serial Monitor output
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C | Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  // Blynk Cloud output
  Blynk.virtualWrite(V5, temp);
  Blynk.virtualWrite(V6, hum);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendDHT); // DHT11 needs 2 sec
}

void loop() {
  Blynk.run();
  timer.run();
}
