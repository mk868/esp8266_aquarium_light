#include "L1Model.h"

#include <ArduinoJson.h>

void L1Model::serialize(String& out){
    StaticJsonDocument<500> doc;
    doc["enabled"] = this->enabled;
    doc["levelR"] = this->levelR;
    doc["levelG"] = this->levelG;
    doc["levelB"] = this->levelB;

    serializeJson(doc, out);
}

void L1Model::deserialize(String& in){
    StaticJsonDocument<500> doc;
    deserializeJson(doc, in);

    this->enabled = doc["enabled"];
    this->levelR = doc["levelR"];
    this->levelG = doc["levelG"];
    this->levelB = doc["levelB"];
}
