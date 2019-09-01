#include <Arduino.h>

#include "models/BaseModel.h"

#ifndef SettingsModel_class_h
#define SettingsModel_class_h

class SettingsModel : public BaseModel {
    public:
        char wifi_ssid[64];
        char wifi_password[64];

        char ntpHost[64];
        long utcOffset;

        char turn_on_L0[9];
        char turn_off_L0[9];

        void toJson(JsonDocument&);
        void fromJson(JsonDocument&);
};

#endif
