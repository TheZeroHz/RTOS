#define utility_core 0
#define app_core 1
#define serial_stack 1200
#define wifi_stack 3000
TaskHandle_t serial_Task = NULL, wifi_Task = NULL;
BaseType_t stask_return, wtask_return;


void task_serial(void *parameter) {
  while (true) {
    if (Serial.available() > 0) {
      String Dataa = Serial.readString();
      if (Dataa == "deletetaskSerial"){Serial.println("Deleting Serial Task");vTaskDelete(serial_Task);}
      else if (Dataa == "deletetaskWifi"){ Serial.println("Deleting Wifi Task");vTaskDelete(wifi_Task);}
    }
    Serial.println("Running Serial Task");
    vTaskDelay(1);
  }
}


void task_wifi(void *parameter) {
  while (true) {
    Serial.println("Running Wifi Task");
    vTaskDelay(1);
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
