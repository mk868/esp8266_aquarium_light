#include <ArduinoJson.h>

#ifndef SerializableModel_class_h
#define SerializableModel_class_h

class SerializableModel
{
public:
    virtual void fromJson(JsonDocument &doc) = 0;
    virtual void toJson(JsonDocument &doc) = 0;
};

#endif
