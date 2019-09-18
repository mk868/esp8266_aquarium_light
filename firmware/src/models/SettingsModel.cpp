#include "models/SettingsModel.h"

SettingsModel::SettingsModel()
{
}

SettingsModel::~SettingsModel()
{
}

void SettingsModel::toJson(JsonDocument &doc)
{
    doc.clear();
    
    doc["night_mode_enabled"] = (int)this->night_mode_enabled;
    doc["night_mode_start"] = this->night_mode_start;
    doc["night_mode_end"] = this->night_mode_end;
};

void SettingsModel::fromJson(JsonDocument &doc)
{
    this->night_mode_enabled = (NightModeEnabledEnum)doc["night_mode_enabled"].as<int>();

    strncpy(this->night_mode_start, doc["night_mode_start"], sizeof(this->night_mode_start));
    strncpy(this->night_mode_end, doc["night_mode_end"], sizeof(this->night_mode_end));
}
