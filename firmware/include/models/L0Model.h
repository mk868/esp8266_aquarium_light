#include <Arduino.h>

#include "models/BaseModel.h"

#ifndef L0Model_class_h
#define L0Model_class_h

class L0Model : public BaseModel {
    public:
        bool enabled;
        int level;

        void toJson(JsonDocument&);
        void fromJson(JsonDocument&);
};

#endif
