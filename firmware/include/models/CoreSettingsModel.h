#include <Arduino.h>

#include <SerializableModel.h>

#ifndef CoreSettingsModel_class_h
#define CoreSettingsModel_class_h

class CoreSettingsModelAPEntry
{
public:
    CoreSettingsModelAPEntry(const char *ssid, const char *password);
    ~CoreSettingsModelAPEntry();

    char *ssid = nullptr;
    char *password = nullptr;
};

class CoreSettingsModel : public SerializableModel
{
public:
    CoreSettingsModel();
    ~CoreSettingsModel();

    //TODO char array -> char*
    char ntpHost[64];

    char DDNSInitUrl[70];

    void toJson(JsonDocument &);
    void fromJson(JsonDocument &);

    std::vector<CoreSettingsModelAPEntry *> APlist; //TODO should be private...
private:
    void APlistClean();
};

#endif
