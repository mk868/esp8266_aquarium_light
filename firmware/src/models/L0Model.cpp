#include "models/L0Model.h"

#include <ArduinoJson.h>

void L0Model::toJson(JsonDocument& doc){
    doc["enabled"] = this->enabled;
    doc["level"] = this->level;
}

void L0Model::fromJson(JsonDocument& doc){
    this->enabled = doc["enabled"];
    this->level = doc["level"];
}
