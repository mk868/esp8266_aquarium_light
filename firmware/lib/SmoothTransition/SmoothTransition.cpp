#include "SmoothTransition.h"

#include <Arduino.h>

SmoothTransition::SmoothTransition(int speed, int defaultValue)
{
    this->startValue = defaultValue;
    this->targetValue = defaultValue;
    this->currentValue = defaultValue;

    this->speed = speed;
}

SmoothTransition::~SmoothTransition()
{
}

void SmoothTransition::setTargetValue(int targetValue)
{
    this->targetValue = targetValue;
    this->startValue = this->currentValue;

    this->transitionStarted = millis();
}

int SmoothTransition::getCurrentValue()
{
    unsigned long now = millis();

    if (now > transitionStarted + speed)
    {
        this->currentValue = this->targetValue;
        return this->currentValue;
    }

    int deltaValue = this->targetValue - this->startValue;
    int elapsedTime = now - transitionStarted;

    this->currentValue = this->startValue + deltaValue * elapsedTime / this->speed;

    return this->currentValue;
}

bool SmoothTransition::isFinished()
{
    return this->currentValue == this->targetValue;
}