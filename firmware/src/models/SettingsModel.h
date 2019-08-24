#include <Arduino.h>

#ifndef SettingsModel_class_h
#define SettingsModel_class_h

class SettingsModel {
    public:
        String wifi_ssid;
        String wifi_password;
        
        String ntpHost;

        unsigned int turn_on_L0;
        unsigned int turn_off_L0;

        void serialize(String& output);
        void deserialize(String& input);
};

#endif
