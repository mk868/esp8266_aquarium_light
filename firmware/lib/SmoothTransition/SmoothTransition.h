#ifndef SmoothTransition_class_h
#define SmoothTransition_class_h

#include <Arduino.h>

class SmoothTransition
{
public:
    void begin(uint8_t pin, int initValue = 0);
    void setValue(int targetValue, int speed);
    void update();
    bool isFinished();

private:
    uint8_t pin;
    int currentValue;
    int startValue;
    int targetValue;
    unsigned long transitionStartTime;
    unsigned long transitionEndTime;
    int getCurrentValue();
};

#endif