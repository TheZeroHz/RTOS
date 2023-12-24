#include "Display.h"

void Display::setPins(int dataPin, int clockPin, int latchPin, int numOFcells) {
  Settings.DS = dataPin;
  Settings.STCP = latchPin;
  Settings.SHCP = clockPin;
  Settings.quantity = numOFcells;
  Settings.state = DEFAULT_STATE;
  Settings.burning_coefficient = DEFAULT_BURNINGFACTOR;
  Settings.cooling_coefficient = DEFAULT_COOLINGFACTOR;
}

void Display::initDisplay(int refresh_rate) {
  Settings.frequency = refresh_rate;
  pinMode(Settings.DS, OUTPUT);
  pinMode(Settings.STCP, OUTPUT);
  pinMode(Settings.SHCP, OUTPUT);
}


void Display::setFactor(float cool, float burn) {
  Settings.burning_coefficient = burn;
  Settings.cooling_coefficient = cool;
}


float Display::getVersion() {
  return version;
}

int Display::getFrequency() {
  return Settings.frequency;
}

void Display::setFrequency(int hz) {
  Settings.frequency = hz;
}

bool Display::isEnabled() {
  return Settings.state;
}

void Display::enable() {
  Settings.state = DISPLAY_ENABLE;
}

void Display::disable() {
  writeDec(IDLE_CODE);
  Settings.state = DISPLAY_DISABLE;
}


int Display::binstrToint(String binary_string) {
  return strtol(binary_string.c_str(), NULL, 2);
}

void Display::pushToRegister(int base_10) {
  Serial.print("Push:");
  Serial.println(base_10);
  if (Settings.state == DISPLAY_ENABLE) {
    digitalWrite(Settings.STCP, LOW);
    shiftOut(Settings.DS, Settings.SHCP, LSBFIRST, base_10);
    digitalWrite(Settings.STCP, HIGH);
  }
}

void Display::clear() {
  pushToRegister(RESET_CODE);
}

void Display::writeDec(int DATA_DECIMAL) {
  float timer = 1000.0 / ((Settings.burning_coefficient + Settings.cooling_coefficient) * Settings.frequency);
  pushToRegister(DATA_DECIMAL);
  delay((int)(timer * Settings.burning_coefficient));
  pushToRegister(IDLE_CODE);
  delay((int)(timer * Settings.cooling_coefficient));
}

void Display::writeBinStr(String DATA_BINARY_STRING) {
  printData=DATA_BINARY_STRING;
}


void Display::loop(){
if(DISPLAY_ENABLE){
writeDec(binstrToint(printData));
}
else{
Serial.println("Display Is Resting...");
}

}
