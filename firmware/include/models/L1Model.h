#include <Arduino.h>

#include <SerializableModel.h>

#ifndef L1Model_class_h
#define L1Model_class_h

class L1Model : public SerializableModel
{
public:
    L1Model();
    ~L1Model();

    bool enabled;
    int levelR;
    int levelG;
    int levelB;

    void toJson(JsonDocument &);
    void fromJson(JsonDocument &);
};

#endif
