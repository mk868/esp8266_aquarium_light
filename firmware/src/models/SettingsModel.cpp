#include "SettingsModel.h"

#include <ArduinoJson.h>

void SettingsModel::serialize(String& out){
    StaticJsonDocument<500> doc;
    doc["wifi_ssid"] = this->wifi_ssid;
    doc["wifi_password"] = this->wifi_password;
    doc["ntpHost"] = this->ntpHost;
    doc["turn_on_L0"] = this->turn_on_L0;
    doc["turn_off_L0"] = this->turn_off_L0;

    serializeJson(doc, out);
};

void SettingsModel::deserialize(String& in){
    StaticJsonDocument<500> doc;
    deserializeJson(doc, in);

    this->wifi_ssid = doc["wifi_ssid"].as<String>();
    this->wifi_password = doc["wifi_password"].as<String>();
    this->ntpHost = doc["ntpHost"].as<String>();
    this->turn_on_L0 = doc["turn_on_L0"];
    this->turn_off_L0 = doc["turn_off_L0"];
}
