
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <SD.h>

#include "models/L0Model.h"
#include "models/L1Model.h"

void WebAppBegin(int port = 80);
void WebAppLoop();
void WebAppEnd();
void WebAppSetL0Model(L0Model*);
void WebAppSetL1Model(L1Model*);

void WebAppOnAPIUpdate(std::function<void(void)>);