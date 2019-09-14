#include <Arduino.h>

#include <SerializableModel.h>

#ifndef SettingsModel_class_h
#define SettingsModel_class_h

class SettingsModelAPEntry {
    public:
        SettingsModelAPEntry(const char *ssid, const char *password);
        ~SettingsModelAPEntry();

        char* ssid = nullptr;
        char* password = nullptr;
};

typedef std::vector<SettingsModelAPEntry*> SettingsModelAPList;

class SettingsModel : public SerializableModel {
    public:
        SettingsModel();
        ~SettingsModel();

        //TODO char array -> char*
        char ntpHost[64];
        long utcOffset;

        char turn_on_L0[9];
        char turn_off_L0[9];

        char DDNSInitUrl[70];

        void toJson(JsonDocument&);
        void fromJson(JsonDocument&);

        SettingsModelAPList APlist; //TODO should be private...
    private:
        void APlistClean();
};

#endif
