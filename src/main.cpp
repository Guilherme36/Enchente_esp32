#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NewPing.h>
#include "soc/rtc_wdt.h"

//============================= Wi-Fi =====================================
const char *ssid = "CASA";
const char *password = "test";
const char *endpoint = "https://7ut960fubc.execute-api.us-east-1.amazonaws.com/leituras";

//============================= Ultrassônicos =====================================
#define TRIGGER_PIN_A 21
#define ECHO_PIN_A    22
#define TRIGGER_PIN_B 25
#define ECHO_PIN_B    33
#define MAX_DISTANCE 200
NewPing sonarA(TRIGGER_PIN_A, ECHO_PIN_A, MAX_DISTANCE);
NewPing sonarB(TRIGGER_PIN_B, ECHO_PIN_B, MAX_DISTANCE);

//============================= Protótipos =====================================
void sendLeitura(int sensorId, float nivelAgua);
String getISO8601Timestamp();

TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code(void *pvParameters);
void Task2code(void *pvParameters);

void setup() {
  disableCore0WDT();
  disableCore1WDT();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("WiFi conectado!");
  Serial.println(WiFi.localIP());

  // Sincronização NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Hora sincronizada");

  xTaskCreatePinnedToCore(Task1code, "Sensor A", 10000, NULL, 1, &Task1, 0);
  xTaskCreatePinnedToCore(Task2code, "Sensor B", 10000, NULL, 1, &Task2, 1);
}

void loop() {}

void Task1code(void *pvParameters) {
  for (;;) {
    float distancia = sonarA.ping_cm();
    Serial.print("[CORE 0] Distância A: "); Serial.println(distancia);
    if (WiFi.status() == WL_CONNECTED) {
      sendLeitura(2, distancia);
    }
    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}

void Task2code(void *pvParameters) {
  for (;;) {
    float distancia = sonarB.ping_cm();
    Serial.print("[CORE 1] Distância B: "); Serial.println(distancia);
    if (WiFi.status() == WL_CONNECTED) {
      sendLeitura(3, distancia);
    }
    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}

void sendLeitura(int sensorId, float nivelAgua) {
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"sensorId\":" + String(sensorId) + ",";
  json += "\"nivelAgua\":" + String(nivelAgua) + ",";
  json += "\"dataHora\":\"" + getISO8601Timestamp() + "\"}";

  int httpResponseCode = http.POST(json);
  Serial.print("POST resposta: "); Serial.println(httpResponseCode);

  if (httpResponseCode < 200 || httpResponseCode >= 300) {
    String payload = http.getString();
    Serial.println("Erro ao enviar leitura.");
    Serial.println("Resposta do servidor: " + payload);
  }
  http.end();
}

String getISO8601Timestamp() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "";
  }
  char timestamp[25];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(timestamp);
}
