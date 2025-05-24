#include <Arduino.h>
#include <NewPing.h>

//============================= Ultrassônicos =====================================
#define TRIGGER_PIN_A 21
#define ECHO_PIN_A    22
#define TRIGGER_PIN_B 25
#define ECHO_PIN_B    33
#define MAX_DISTANCE 200

NewPing sonarA(TRIGGER_PIN_A, ECHO_PIN_A, MAX_DISTANCE);
NewPing sonarB(TRIGGER_PIN_B, ECHO_PIN_B, MAX_DISTANCE);

//============================= SETUP =====================================
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando teste dos sensores HC-SR04...");
}

//============================= LOOP =====================================
void loop() {
  float distanciaA = sonarA.ping_cm();
  float distanciaB = sonarB.ping_cm();

  Serial.print("Distância Sensor A: ");
  Serial.print(distanciaA);
  Serial.println(" cm");

  Serial.print("Distância Sensor B: ");
  Serial.print(distanciaB);
  Serial.println(" cm");

  delay(1000); // Atualiza a cada 1 segundo
}
