#include <ArduinoJson.h>

#ifndef BaseModel_class_h
#define BaseModel_class_h

class BaseModel {
    public:
        virtual void fromJson(JsonDocument &doc) = 0;
        virtual void toJson(JsonDocument &doc) = 0;
};

#endif
