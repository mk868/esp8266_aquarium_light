#include "L0Model.h"

#include <ArduinoJson.h>

void L0Model::serialize(String& out){
    StaticJsonDocument<500> doc;
    doc["enabled"] = this->enabled;
    doc["level"] = this->level;

    serializeJson(doc, out);
}

void L0Model::deserialize(String& in){
    StaticJsonDocument<500> doc;
    deserializeJson(doc, in);

    this->enabled = doc["enabled"];
    this->level = doc["level"];
}
