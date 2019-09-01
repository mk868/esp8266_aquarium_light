#include "models/SettingsModel.h"

void SettingsModel::toJson(JsonDocument &doc)
{
    doc["wifi_ssid"] = this->wifi_ssid;
    doc["wifi_password"] = this->wifi_password;

    doc["ntpHost"] = this->ntpHost;
    doc["utcOffset"] = this->utcOffset;

    doc["turn_on_L0"] = this->turn_on_L0;
    doc["turn_off_L0"] = this->turn_off_L0;
};

void SettingsModel::fromJson(JsonDocument &doc)
{
    strncpy(this->wifi_ssid, doc["wifi_ssid"], sizeof(this->wifi_ssid));
    strncpy(this->wifi_password, doc["wifi_password"], sizeof(this->wifi_password));

    strncpy(this->ntpHost, doc["ntpHost"], sizeof(this->ntpHost));
    this->utcOffset = doc["utcOffset"];

    strncpy(this->turn_on_L0, doc["turn_on_L0"], sizeof(this->turn_on_L0));
    strncpy(this->turn_off_L0, doc["turn_off_L0"], sizeof(this->turn_off_L0));
}
