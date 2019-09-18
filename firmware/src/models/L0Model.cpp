#include "models/L0Model.h"

L0Model::L0Model()
{
}

L0Model::~L0Model()
{
}

void L0Model::toJson(JsonDocument &doc)
{
    doc.clear();
    doc["enabled"] = this->enabled;
    doc["level"] = this->level;
}

void L0Model::fromJson(JsonDocument &doc)
{
    this->enabled = doc["enabled"];
    this->level = doc["level"];
}
