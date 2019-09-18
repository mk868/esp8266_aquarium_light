#include <Arduino.h>
#include "SerializableModel.h"

#ifndef SDSerializer_class_h
#define SDSerializer_class_h

class SDSerializer
{
public:
    SDSerializer(int bufferSize = 500) : bufferSize(bufferSize){};
    bool load(String path, SerializableModel *model);
    bool save(String path, SerializableModel *model, bool pretty = false);

private:
    int bufferSize;
};

#endif
