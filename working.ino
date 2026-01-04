#define BLYNK_PRINT Serial

// -------- Blynk Template Details --------
#define BLYNK_TEMPLATE_ID "TMPL3Y_ija-3b"
#define BLYNK_TEMPLATE_NAME "Mushroom Monitoring"
#define BLYNK_AUTH_TOKEN "ntfhMVnlLmG7uNPYj4YhkwctrGGF8_dS"

// -------- Libraries --------
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// -------- WiFi Credentials --------
char ssid[] = "POCO C55";
char pass[] = "sampathkumar";

// -------- Pin Definitions --------
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 35
#define LDR_PIN 34

// -------- Thresholds --------
#define TEMP_MIN 20
#define TEMP_MAX 32
#define CO2_MIN 800
#define CO2_MAX 1200
#define LDR_THRESHOLD 2000
#define TEMP_LED_PIN 2


DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// -------- Read & Send Sensor Data --------
void sendSensorData() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ DHT11 read failed");
    return;
  }

  int mq135Value = analogRead(MQ135_PIN);
  int co2ppm = map(mq135Value, 0, 4095, 400, 2000);

  int ldrValue = analogRead(LDR_PIN);

  // -------- Serial Monitor --------
  if (temperature > 32) {
  digitalWrite(TEMP_LED_PIN, HIGH);   // LED ON
  Serial.println("🔥 TEMP HIGH - LED ON");
} else {
  digitalWrite(TEMP_LED_PIN, LOW);    // LED OFF
}

  Serial.println("========== SENSOR DATA ==========");
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("CO2: ");
  Serial.print(co2ppm);
  Serial.println(" ppm");

  Serial.print("LDR Status: ");
  Serial.println((ldrValue > LDR_THRESHOLD) ? "LIGHT" : "DARK");
  Serial.println("================================");


  Blynk.virtualWrite(V0, temperature);  // Temp
  Blynk.virtualWrite(V1, humidity);     // Humidity
  Blynk.virtualWrite(V2, co2ppm);       // CO2

  // LDR LED ONLY (V3)
  if (ldrValue > LDR_THRESHOLD)
    Blynk.virtualWrite(V3, 255); // LIGHT
  else
    Blynk.virtualWrite(V3, 0);   // DARK
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, sendSensorData);
  pinMode(TEMP_LED_PIN, OUTPUT);
  digitalWrite(TEMP_LED_PIN, LOW);

}

// -------- Loop --------
void loop() {
  Blynk.run();
  timer.run();
}
