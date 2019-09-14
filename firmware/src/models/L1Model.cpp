#include "models/L1Model.h"

L1Model::L1Model(){
}

L1Model::~L1Model(){
}

void L1Model::toJson(JsonDocument& doc){
    doc["enabled"] = this->enabled;
    doc["levelR"] = this->levelR;
    doc["levelG"] = this->levelG;
    doc["levelB"] = this->levelB;
}

void L1Model::fromJson(JsonDocument& doc){
    this->enabled = doc["enabled"];
    this->levelR = doc["levelR"];
    this->levelG = doc["levelG"];
    this->levelB = doc["levelB"];
}
