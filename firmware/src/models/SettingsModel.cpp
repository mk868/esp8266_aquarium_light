#include "models/SettingsModel.h"

SettingsModelAPEntry::SettingsModelAPEntry(const char *ssid, const char *password)
{
    this->ssid = (char *)malloc(strlen(ssid) + 1); // TODO if null...
    this->password = (char *)malloc(strlen(password) + 1);

    strncpy(this->ssid, ssid, strlen(ssid) + 1);
    strncpy(this->password, password, strlen(password) + 1);
}

SettingsModelAPEntry::~SettingsModelAPEntry()
{
    if (this->ssid != nullptr)
    {
        free(this->ssid);
        this->ssid = nullptr;
    }
    if (this->password != nullptr)
    {
        free(this->password);
        this->password = nullptr;
    }
}

SettingsModel::SettingsModel()
{
}

SettingsModel::~SettingsModel()
{
    APlistClean();
}

void SettingsModel::toJson(JsonDocument &doc)
{
    JsonArray apsArray = doc.createNestedArray("APs");
    for(auto entry : APlist) {
        JsonObject apInfo = apsArray.createNestedObject();
        apInfo["ssid"] = entry->ssid;
        apInfo["password"] = entry->password;
    }

    doc["ntpHost"] = this->ntpHost;
    doc["utcOffset"] = this->utcOffset;

    doc["DDNSInitUrl"] = this->DDNSInitUrl;

    doc["turn_on_L0"] = this->turn_on_L0;
    doc["turn_off_L0"] = this->turn_off_L0;
};

void SettingsModel::fromJson(JsonDocument &doc)
{
    APlistClean();

    JsonArray apsArray = doc["APs"];
    for(auto apInfo : apsArray) {
        const char* ssid = apInfo["ssid"];
        const char* password = apInfo["password"];
        
        auto entry = new SettingsModelAPEntry(ssid, password);
        this->APlist.push_back(entry);
    }
  
    strncpy(this->ntpHost, doc["ntpHost"], sizeof(this->ntpHost));
    this->utcOffset = doc["utcOffset"];

    strncpy(this->DDNSInitUrl, doc["DDNSInitUrl"], sizeof(this->DDNSInitUrl));

    strncpy(this->turn_on_L0, doc["turn_on_L0"], sizeof(this->turn_on_L0));
    strncpy(this->turn_off_L0, doc["turn_off_L0"], sizeof(this->turn_off_L0));
}

void SettingsModel::APlistClean()
{
    for(auto entry : APlist) {
        delete entry;
    }
    APlist.clear();
}
