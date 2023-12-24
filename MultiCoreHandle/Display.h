#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>  // Include this if you are using Arduino platform

#define DISPLAY_ENABLE true
#define DISPLAY_DISABLE false
#define DEFAULT_STATE DISPLAY_ENABLE
#define DEFAULT_FREQUENCY 30
#define DEFAULT_COOLINGFACTOR 4
#define DEFAULT_BURNINGFACTOR 0.5
#define RESET_CODE 128
#define IDLE_CODE 0

struct DisplaySettings {
    int STCP, DS, SHCP, frequency, quantity;
    bool state;
    float cooling_coefficient, burning_coefficient;
};

class Display {
protected:
    float version = 1.01;
    DisplaySettings Settings;
    void pushToRegister(int base_10);
    int binstrToint(String binary_string);

public:
    String printData="";
    void loop();
    void setPins(int dataPin, int clockPin, int latchPin, int numOFcells);
    void initDisplay(int refresh_rate);
    void disable();
    void enable();
    void setFrequency(int hz);
    void setFactor(float cool, float burn);
    bool isEnabled();
    float getVersion();
    int getFrequency();
    void clear();
    void writeDec(int DATA_DECIMAL);
    void writeBinStr(String DATA_BINARY_STRING);
    void writeChar(char DATA_CHAR);
};

#endif // DISPLAY_H
