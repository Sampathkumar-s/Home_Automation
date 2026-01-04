#define BLYNK_PRINT Serial

// -------- Blynk Template Details --------
#define BLYNK_TEMPLATE_ID "TMPL3Y_ija-3b"
#define BLYNK_TEMPLATE_NAME "Mushroom Monitoring"
#define BLYNK_AUTH_TOKEN "ntfhMVnlLmG7uNPYj4YhkwctrGGF8_dS"

// -------- Libraries --------
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <HTTPClient.h>

// -------- WiFi Credentials --------
char ssid[] = "POCO C55";
char pass[] = "sampathkumar";

// -------- Pin Definitions --------
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 35
#define LDR_PIN 34
#define TEMP_LED_PIN 2

// -------- Thresholds --------
#define TEMP_MAX 32
#define CO2_MAX 1200
#define LDR_THRESHOLD 2000

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// -------- Telegram Info --------
const String BOT_TOKEN = "8502068886:AAFXpmnKL_gdRCXVuzqzOYAWbp7cXo4PErE";
const String CHAT_ID  = "1544122708";

// -------- Flags to prevent spamming --------
bool tempAlertSent = false;
bool co2AlertSent = false;
bool ldrAlertSent = false;

// -------- Function to send Telegram message --------
void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + message;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Telegram message sent: " + message);
    } else {
      Serial.println("Error sending Telegram message");
    }
    http.end();
  }
}

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

  // -------- Temperature Alert with LED & Telegram --------
  if (temperature > TEMP_MAX) {
    digitalWrite(TEMP_LED_PIN, HIGH);   // LED ON
    Serial.println("🔥 TEMP HIGH - LED ON");

    if (!tempAlertSent) {
      String msg = "⚠️ Temperature Alert! Current Temp = " + String(temperature) + "°C";
      sendTelegramMessage(msg);
      tempAlertSent = true;
    }
  } else {
    digitalWrite(TEMP_LED_PIN, LOW);    // LED OFF
    tempAlertSent = false;
  }

  // -------- CO2 Alert --------
  if (co2ppm > CO2_MAX) {
    if (!co2AlertSent) {
      String msg = "⚠️ CO2 Alert! Current CO2 = " + String(co2ppm) + " ppm";
      sendTelegramMessage(msg);
      co2AlertSent = true;
    }
  } else {
    co2AlertSent = false;
  }

  // -------- LDR Alert --------
  if (ldrValue > LDR_THRESHOLD) {
    if (!ldrAlertSent) {
      String msg = "💡 LDR Alert! Light detected. Value = " + String(ldrValue);
      sendTelegramMessage(msg);
      ldrAlertSent = true;
    }
  } else {
    if (ldrAlertSent) {
      String msg = "🌑 LDR Alert! Dark detected. Value = " + String(ldrValue);
      sendTelegramMessage(msg);
      ldrAlertSent = false;
    }
  }

  // -------- Serial Monitor --------
  Serial.println("========== SENSOR DATA ==========");
  Serial.print("Temp: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("CO2: "); Serial.println(co2ppm);
  Serial.print("LDR Status: "); Serial.println((ldrValue > LDR_THRESHOLD) ? "LIGHT" : "DARK");
  Serial.println("================================");

  // -------- Send data to Blynk --------
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
  pinMode(TEMP_LED_PIN, OUTPUT);
  digitalWrite(TEMP_LED_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, sendSensorData); // read every 3 seconds
}

// -------- Loop --------
void loop() {
  Blynk.run();
  timer.run();
}
