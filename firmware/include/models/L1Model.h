#include <Arduino.h>

#include "models/BaseModel.h"

#ifndef L1Model_class_h
#define L1Model_class_h

class L1Model : public BaseModel {
    public:
        bool enabled;
        int levelR;
        int levelG;
        int levelB;

        void toJson(JsonDocument&);
        void fromJson(JsonDocument&);
};

#endif
