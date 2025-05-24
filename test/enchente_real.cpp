//============================= Bibliotecas =====================================
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <NewPing.h>

//============================= Wi-Fi =====================================
const char *ssid = "CASA";
const char *password = "test";

//============================= ENDPOINT =====================================
const char *endpoint = "https://7ut960fubc.execute-api.us-east-1.amazonaws.com/leituras";

//============================= Ultrassônicos =====================================
#define TRIGGER_PIN_A 21
#define ECHO_PIN_A    22
#define TRIGGER_PIN_B 25
#define ECHO_PIN_B    33
#define MAX_DISTANCE 200
NewPing sonarA(TRIGGER_PIN_A, ECHO_PIN_A, MAX_DISTANCE);
NewPing sonarB(TRIGGER_PIN_B, ECHO_PIN_B, MAX_DISTANCE);

//============================= PROTÓTIPOS =====================================
void sendLeitura(int sensorId, float nivelAgua);
String getISO8601Timestamp();

//============================= SETUP =====================================
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("WiFi conectado!");
  Serial.println(WiFi.localIP());

  // Sincronização NTP - UTC
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Sincronizando hora");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" OK");
}

//============================= LOOP =====================================
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float distanciaA = sonarA.ping_cm();
    float distanciaB = sonarB.ping_cm();

    Serial.print("Distância A: "); Serial.println(distanciaA);
    Serial.print("Distância B: "); Serial.println(distanciaB);

    sendLeitura(2, distanciaA);
    sendLeitura(3, distanciaB);
  } else {
    Serial.println("WiFi desconectado");
  }
  delay(30000); // 30 segundos
}

//============================= FUNÇÃO: Enviar Leitura ===============================
void sendLeitura(int sensorId, float nivelAgua) {
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"sensorId\":" + String(sensorId) + ",";
  json += "\"nivelAgua\":" + String(nivelAgua) + ",";
  json += "\"dataHora\":\"" + getISO8601Timestamp() + "\"}";

  int httpResponseCode = http.POST(json);

  Serial.print("POST resposta: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode >= 200 && httpResponseCode < 300) {
    Serial.println("Leitura enviada com sucesso.");
  } else {
    Serial.println("Erro ao enviar leitura.");
    String payload = http.getString();
    Serial.print("Resposta do servidor: ");
    Serial.println(payload);
  }

  http.end();
}

//============================= Timestamp ISO 8601 =============================
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
