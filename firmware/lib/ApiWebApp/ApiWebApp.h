
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <SerializableModel.h>

void ApiWebAppBegin(ESP8266WebServer *webServer);
void ApiWebAppAddModelApi(const char *path, SerializableModel *model, std::function<void(void)> afterUpdate);