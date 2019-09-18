#include <Arduino.h>

#include <SerializableModel.h>

#ifndef SettingsModel_class_h
#define SettingsModel_class_h

enum NightModeEnabledEnum { Auto, On, Off };

class SettingsModel : public SerializableModel
{
public:
    SettingsModel();
    ~SettingsModel();

    NightModeEnabledEnum night_mode_enabled;

    char night_mode_start[9];
    char night_mode_end[9];
    
    void toJson(JsonDocument &);
    void fromJson(JsonDocument &);
};

#endif
