#include <arduino.h>
#include "soc/rtc_wdt.h"
TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code(void *pvParameters);
void Task2code(void *pvParameters);

void setup()
{
  disableCore0WDT();
  disableCore1WDT();

  Serial.begin(115200);
  // Cria uma tarefa que será executada na função Task1code(), com  prioridade 1 e execução no núcleo 0
  xTaskCreatePinnedToCore(
      Task1code,  /* Função da tarefa */
      "Lembrete", /* nome da tarefa */
      10000,      /* Tamanho (bytes) */
      NULL,       /* parâmetro da tarefa */
      1,          /* prioridade da tarefa */
      &Task1,     /* observa a tarefa criada */
      0);         /* tarefa alocada ao núcleo 0 */
  // Cria uma tarefa que será executada na função Task2code(), com  prioridade 1 e execução no núcleo 1
  xTaskCreatePinnedToCore(
      Task2code,    /* Função da tarefa */
      "Ventilador", /* nome da tarefa */
      10000,        /* Tamanho (bytes) */
      NULL,         /* parâmetro da tarefa */
      1,            /* prioridade da tarefa */
      &Task2,       /* observa a tarefa criada */
      1);           /* tarefa alocada ao núcleo 1 */
}
// Task1code: Lembrete para beber água
void Task1code(void *pvParameters)
{
  for (;;)
  {
  }
}
// Task2code: Controle do ventilador
void Task2code(void *pvParameters)
{
  for (;;)
  { // Loop infinito
    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}
void loop()
{
}