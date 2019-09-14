#include "SDSerializer.h"
#include "SPI.h"
#include "SD.h"

bool SDSerializer::load(String path, SerializableModel *model)
{
    DynamicJsonDocument doc(this->bufferSize);
    Serial.println(String("Loading file: ") + path);
    if (!SD.exists(path))
    {
        Serial.println("File doesn't exist");
        return false;
    }

    File file = SD.open(path, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        Serial.print("Failed to read file: ");
        Serial.println(path);
        Serial.print("JSON Error: ");
        Serial.println(error.c_str());
        return false;
    }

    model->fromJson(doc);
    return true;
}

bool SDSerializer::save(String path, SerializableModel *model)
{
    DynamicJsonDocument doc(this->bufferSize);
    if (SD.exists(path))
    {
        SD.remove(path);
    }
    File file = SD.open(path, FILE_WRITE);
    model->toJson(doc);
    serializeJson(doc, file);
    file.close();
    return true;
}