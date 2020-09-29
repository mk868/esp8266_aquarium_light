#include "models/CoreSettingsModel.h"

CoreSettingsModelAPEntry::CoreSettingsModelAPEntry(const char *ssid, const char *password)
{
    this->ssid = (char *)malloc(strlen(ssid) + 1); // TODO if null...
    this->password = (char *)malloc(strlen(password) + 1);

    strncpy(this->ssid, ssid, strlen(ssid) + 1);
    strncpy(this->password, password, strlen(password) + 1);
}

CoreSettingsModelAPEntry::~CoreSettingsModelAPEntry()
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

CoreSettingsModel::CoreSettingsModel()
{
}

CoreSettingsModel::~CoreSettingsModel()
{
    APlistClean();
}

void CoreSettingsModel::toJson(JsonDocument &doc)
{
    doc.clear();
    JsonArray apsArray = doc.createNestedArray("APs");
    for (auto entry : APlist)
    {
        JsonObject apInfo = apsArray.createNestedObject();
        apInfo["ssid"] = entry->ssid;
        apInfo["password"] = entry->password;
    }

    doc["ntpHost"] = this->ntpHost;
    doc["DDNSInitUrl"] = this->DDNSInitUrl;
};

void CoreSettingsModel::fromJson(JsonDocument &doc)
{
    APlistClean();

    JsonArray apsArray = doc["APs"];
    for (auto apInfo : apsArray)
    {
        const char *ssid = apInfo["ssid"];
        const char *password = apInfo["password"];

        auto entry = new CoreSettingsModelAPEntry(ssid, password);
        this->APlist.push_back(entry);
    }

    strncpy(this->ntpHost, doc["ntpHost"], sizeof(this->ntpHost));
    strncpy(this->DDNSInitUrl, doc["DDNSInitUrl"], sizeof(this->DDNSInitUrl));
}

void CoreSettingsModel::APlistClean()
{
    for (auto entry : APlist)
    {
        delete entry;
    }
    APlist.clear();
}
