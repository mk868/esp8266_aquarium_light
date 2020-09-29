#include "SmoothTransition.h"

#include <Arduino.h>

void SmoothTransition::begin(uint8_t pin, int initValue)
{
    this->pin = pin;
    this->startValue = initValue;
    this->targetValue = initValue;
    this->currentValue = initValue;

    pinMode(pin, OUTPUT);
    analogWrite(pin, initValue);
}

void SmoothTransition::setValue(int targetValue, int speed)
{
    this->targetValue = targetValue;
    this->startValue = this->currentValue;

    this->transitionStartTime = millis();
    this->transitionEndTime = this->transitionStartTime + speed;
}

int SmoothTransition::getCurrentValue()
{
    unsigned long now = millis();

    if (now > this->transitionEndTime)
    {
        this->currentValue = this->targetValue;
        return this->currentValue;
    }

    int deltaValue = this->targetValue - this->startValue;
    int deltaTime = this->transitionEndTime - this->transitionStartTime;
    int elapsedTime = now - this->transitionStartTime;

    this->currentValue = this->startValue + deltaValue * elapsedTime / deltaTime;

    return this->currentValue;
}

void SmoothTransition::update()
{
    if (!this->isFinished())
    {
        analogWrite(this->pin, this->getCurrentValue());
    }
}

bool SmoothTransition::isFinished()
{
    return this->currentValue == this->targetValue;
}