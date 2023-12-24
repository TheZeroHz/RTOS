#include <WiFi.h>
#include "Display.h"
#include "Audio.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define app_core 1
#define utility_core 0
#define display_stack 2000
#define audio_stack 5000


#define SD_CS 10
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCK 12
// Digital I/O used
#define I2S_DOUT 6
#define I2S_BCLK 5
#define I2S_LRC 4


TaskHandle_t display_handle = NULL;
TaskHandle_t audio_handke = NULL;


Display disp;
Audio audio;


String ssid = "rakib";
String password = "rakib@2023";

//****************************************************************************************
//                                    Display  _ T A S K                                 *
//****************************************************************************************

struct dispMessage {
  uint8_t cmd;
  String txt;
  uint32_t value;
  uint32_t ret;
} dispTxMessage, dispRxMessage;

enum : uint8_t {
  SET_FREQ,
  GET_FREQ,
  DISP_PRINT,
  DISP_DISABLE,
  DISP_ENABLE
};

QueueHandle_t dispSetQueue = NULL;
QueueHandle_t dispGetQueue = NULL;

void dispCreateQueues() {
  dispSetQueue = xQueueCreate(10, sizeof(struct dispMessage));
  dispGetQueue = xQueueCreate(10, sizeof(struct dispMessage));
}

void dispTask(void* parameter) {
  dispCreateQueues();
  if (!dispSetQueue || !dispGetQueue) {
    log_e("queues are not initialized");
    while (true) { ; }  // endless loop
  }

  struct dispMessage dispRxTaskMessage;
  struct dispMessage dispTxTaskMessage;

  disp.setPins(15, 16, 17, 1);
  disp.initDisplay(14);

  while (true) {
    //Serial.println("Hello Boss");
    if (xQueueReceive(dispSetQueue, &dispRxTaskMessage, 1) == pdPASS) {
      if (dispRxTaskMessage.cmd == SET_FREQ) {
        dispTxTaskMessage.cmd = SET_FREQ;
        disp.setFrequency(dispRxTaskMessage.value);
        dispTxTaskMessage.ret = 1;
        xQueueSend(dispGetQueue, &dispTxTaskMessage, portMAX_DELAY);
      } else if (dispRxTaskMessage.cmd == DISP_PRINT) {
        dispTxTaskMessage.cmd = DISP_PRINT;
        disp.writeBinStr(dispRxTaskMessage.txt);
        xQueueSend(dispGetQueue, &dispTxTaskMessage, portMAX_DELAY);
      } else if (dispRxTaskMessage.cmd == GET_FREQ) {
        dispTxTaskMessage.cmd = GET_FREQ;
        dispTxTaskMessage.ret = disp.getFrequency();
        xQueueSend(dispGetQueue, &dispTxTaskMessage, portMAX_DELAY);
      } else if (dispRxTaskMessage.cmd == DISP_ENABLE) {
        dispTxTaskMessage.cmd = DISP_ENABLE;
        disp.enable();
        xQueueSend(dispGetQueue, &dispTxTaskMessage, portMAX_DELAY);
      } else if (dispRxTaskMessage.cmd == DISP_DISABLE) {
        dispTxTaskMessage.cmd = DISP_DISABLE;
        disp.disable();
        xQueueSend(dispGetQueue, &dispTxTaskMessage, portMAX_DELAY);
      } else {
        log_i("error");
      }
    }
    disp.loop();
    if (!disp.isEnabled()) {
      sleep(1);
    }
    // audio.loop();
    // if (!audio.isRunning()) {
    // sleep(1);
    // }
  }
}

void displayInit() {
  xTaskCreatePinnedToCore(
    dispTask,              /* Function to implement the task */
    "display",             /* Name of the task */
    display_stack,         /* Stack size in words */
    NULL,                  /* Task input parameter */
    2 | portPRIVILEGE_BIT, /* Priority of the task */
    &display_handle,       /* Task handle. */
    app_core               /* Core where the task should run */
  );
}

struct dispMessage transmitReceive(dispMessage msg) {
  xQueueSend(dispSetQueue, &msg, portMAX_DELAY);
  if (xQueueReceive(dispGetQueue, &dispRxMessage, portMAX_DELAY) == pdPASS) {
    if (msg.cmd != dispRxMessage.cmd) {
      log_e("wrong reply from message queue");
    }
  }
  return dispRxMessage;
}

void dispSetFreq(uint8_t freq) {
  dispTxMessage.cmd = SET_FREQ;
  dispTxMessage.value = freq;
  dispMessage RX = transmitReceive(dispTxMessage);
}

uint8_t dispGetFreq() {
  dispTxMessage.cmd = GET_FREQ;
  dispMessage RX = transmitReceive(dispTxMessage);
  return RX.ret;
}

void dispEnable() {
  dispTxMessage.cmd = DISP_ENABLE;
  dispMessage RX = transmitReceive(dispTxMessage);
}

void dispDisable() {
  dispTxMessage.cmd = DISP_DISABLE;
  dispMessage RX = transmitReceive(dispTxMessage);
}


void dispPrint(String msg) {
  dispTxMessage.cmd = DISP_PRINT;
  dispTxMessage.txt = msg;
  dispMessage RX = transmitReceive(dispTxMessage);
}

//****************************************************************************************
//                                  END OF DISPLAY PORTION                               *
//****************************************************************************************






//****************************************************************************************
//                                   A U D I O _ T A S K                                 *
//****************************************************************************************

struct audioMessage{
    uint8_t     cmd;
    const char* txt;
    uint32_t    value;
    uint32_t    ret;
} audioTxMessage, audioRxMessage;

enum : uint8_t { SET_VOLUME, GET_VOLUME, CONNECTTOHOST, CONNECTTOSD };

QueueHandle_t audioSetQueue = NULL;
QueueHandle_t audioGetQueue = NULL;

void CreateQueues(){
    audioSetQueue = xQueueCreate(10, sizeof(struct audioMessage));
    audioGetQueue = xQueueCreate(10, sizeof(struct audioMessage));
}

void audioTask(void *parameter) {
    CreateQueues();
    if(!audioSetQueue || !audioGetQueue){
        log_e("queues are not initialized");
        while(true){;}  // endless loop
    }

    struct audioMessage audioRxTaskMessage;
    struct audioMessage audioTxTaskMessage;

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21

    while(true){
        if(xQueueReceive(audioSetQueue, &audioRxTaskMessage, 1) == pdPASS) {
            if(audioRxTaskMessage.cmd == SET_VOLUME){
                audioTxTaskMessage.cmd = SET_VOLUME;
                audio.setVolume(audioRxTaskMessage.value);
                audioTxTaskMessage.ret = 1;
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOHOST){
                audioTxTaskMessage.cmd = CONNECTTOHOST;
                audioTxTaskMessage.ret = audio.connecttohost(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOSD){
                audioTxTaskMessage.cmd = CONNECTTOSD;
                audioTxTaskMessage.ret = audio.connecttoSD(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == GET_VOLUME){
                audioTxTaskMessage.cmd = GET_VOLUME;
                audioTxTaskMessage.ret = audio.getVolume();
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else{
                log_i("error");
            }
        }
        audio.loop();
        if (!audio.isRunning()) {
          sleep(1);
        }
    }
}

void audioInit() {
    xTaskCreatePinnedToCore(
        audioTask,             /* Function to implement the task */
        "audioplay",           /* Name of the task */
        5000,                  /* Stack size in words */
        NULL,                  /* Task input parameter */
        2 | portPRIVILEGE_BIT, /* Priority of the task */
        NULL,                  /* Task handle. */
        1                      /* Core where the task should run */
    );
}
struct audioMessage transmitReceive(audioMessage msg){
    xQueueSend(audioSetQueue, &msg, portMAX_DELAY);
    if(xQueueReceive(audioGetQueue, &audioRxMessage, portMAX_DELAY) == pdPASS){
        if(msg.cmd != audioRxMessage.cmd){
            log_e("wrong reply from message queue");
        }
    }
    return audioRxMessage;
}

void audioSetVolume(uint8_t vol){
    audioTxMessage.cmd = SET_VOLUME;
    audioTxMessage.value = vol;
    audioMessage RX = transmitReceive(audioTxMessage);
}

uint8_t audioGetVolume(){
    audioTxMessage.cmd = GET_VOLUME;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttohost(const char* host){
    audioTxMessage.cmd = CONNECTTOHOST;
    audioTxMessage.txt = host;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttoSD(const char* filename){
    audioTxMessage.cmd = CONNECTTOSD;
    audioTxMessage.txt = filename;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}
//****************************************************************************************
//                                  END OF AUDIO PORTION                                 *
//****************************************************************************************


















//****************************************************************************************
//                                   S E T U P                                           *
//****************************************************************************************

void setup() {
  Serial.begin(115200);
  delay(3000);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  SD.begin(SD_CS);

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  audioInit();
  (5000);
  displayInit();

  log_i("current display frequency is: %d", dispGetFreq());
  log_i("current volume is: %d", audioGetVolume());

}

//****************************************************************************************
//                                   L O O P                                             *
//****************************************************************************************
String dataa;
void loop() {
  // your own code here

  while (Serial.available()) {

    dataa = Serial.readString();
    
    if (dataa.length() > 3) {
      Serial.println("Playing" + dataa);
      if (dataa[0] == 'C') audioConnecttoSD(dataa.c_str());
      else if (dataa[0] =='D') dispDisable();
      else if (dataa[0]== 'E') dispEnable();
      else dispPrint(dataa.c_str());
    } 
    
    else dispSetFreq(dataa.toInt());
  }
}