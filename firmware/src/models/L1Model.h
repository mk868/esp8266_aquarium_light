#include <Arduino.h>

#ifndef L1Model_class_h
#define L1Model_class_h

class L1Model {
    public:
        bool enabled;
        int levelR;
        int levelG;
        int levelB;

        void serialize(String& output);
        void deserialize(String& input);
};

#endif
