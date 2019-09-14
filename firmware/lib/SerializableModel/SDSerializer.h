#include <Arduino.h>
#include "SerializableModel.h"

class SDSerializer
{
public:
    SDSerializer(int bufferSize = 500) : bufferSize(bufferSize){};
    bool load(String path, SerializableModel *model);
    bool save(String path, SerializableModel *model);
private:
    int bufferSize;
};