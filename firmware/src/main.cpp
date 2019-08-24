#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <SD.h>

#include "config.h"
#include "webapp.h"

#include "models/L0Model.h"
#include "models/L1Model.h"
#include "models/SettingsModel.h"

//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/SDWebServer/SDWebServer.ino

L0Model l0;
L1Model l1;
SettingsModel settings;


void updateL(){
    // L0
    if(l0.enabled){
        analogWrite(PIN_L0, 1024 - l0.level);
    } else {
        digitalWrite(PIN_L0, HIGH);
    }

/*
    if(l1.enabled){
        analogWrite(PIN_L1_R, 1024 - l1.levelR);
        analogWrite(PIN_L1_G, 1024 - l1.levelG);
        analogWrite(PIN_L1_B, 1024 - l1.levelB);
    } else {
        digitalWrite(PIN_L1_R, HIGH);
        digitalWrite(PIN_L1_G, HIGH);
        digitalWrite(PIN_L1_B, HIGH);
    }
*/
    Serial.println("update");
}

void initSD(){
    if (!SD.begin(PIN_CS)) {
        Serial.println("Load SD card error!");
    }
}

void loadSettings(){
    File testfile = SD.open(SETTINGS_FILE, FILE_READ);
    String tmp;
    while (testfile.available()) {
        tmp += (char)testfile.read();
    }
    testfile.close();
    settings.deserialize(tmp);
}

void initWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifi_ssid, settings.wifi_password);
    Serial.print("Connecting to ");
    Serial.println(settings.wifi_ssid);

    while(!WiFi.isConnected()){
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
}

void initWebApp(){
    WebAppBegin(80);
    WebAppSetL0Model(&l0);
    WebAppSetL1Model(&l1);
    WebAppOnAPIUpdate(updateL);
}

void initIO(){
    pinMode(PIN_L0, OUTPUT);
}

void setup() {
    Serial.begin(SERIAL_SPEED);
    //Serial.setDebugOutput(true);
    Serial.print("Hello!\n");

    initSD();
    loadSettings();
    initWiFi();
    initWebApp();
    initIO();
}

void loop() {
    WebAppLoop();
}