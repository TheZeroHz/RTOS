#define utility_core 0
#define app_core 1
#define serial_stack 1200
#define wifi_stack 3000
TaskHandle_t serial_Task = NULL, wifi_Task = NULL;
BaseType_t stask_return, wtask_return;


void task_serial(void *parameter) {
  while (true) {
    Serial.println("This is Serial Task");
  }
}

void task_wifi(void *parameter) {
  while (true) {
    Serial.println("This is Wifi Task");
  }
}



void setup() {
  Serial.begin(115200);
  stask_return = xTaskCreatePinnedToCore(task_serial, "CMD", serial_stack, NULL, 5 | portPRIVILEGE_BIT, &serial_Task, app_core);
  wtask_return = xTaskCreatePinnedToCore(task_wifi, "Wifi", wifi_stack, NULL, 5 | portPRIVILEGE_BIT, &wifi_Task, utility_core);
  if (stask_return == pdPASS) {
    Serial.println("CMD Created!");
  }
  if (wtask_return == pdPASS) {
    Serial.println("Wifi Created");
  }
}

void loop() {
}
