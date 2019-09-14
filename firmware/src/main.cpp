#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <SD.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>

#include "config.h"
#include "webapp.h"

#include "models/L0Model.h"
#include "models/L1Model.h"
#include "models/SettingsModel.h"

#include "SmoothTransition.h"

ESP8266WiFiMulti wifiMulti;

L0Model l0;
L1Model l1;
SettingsModel settings;

SmoothTransition l0Value(ANIMATION_SPEED, PWMRANGE);
SmoothTransition l1RValue(ANIMATION_SPEED, PWMRANGE);
SmoothTransition l1GValue(ANIMATION_SPEED, PWMRANGE);
SmoothTransition l1BValue(ANIMATION_SPEED, PWMRANGE);

WiFiUDP ntpUDP;
NTPClient *timeClient;

bool nightMode;
bool timeJobInited = false;
unsigned long makeStateSaveAt = 0;

bool networkActionsInited = false;

void makeStateSave()
{
    StaticJsonDocument<500> doc;

    if (SD.exists(L0_FILE))
    {
        SD.remove(L0_FILE);
    }
    File fileL0 = SD.open(L0_FILE, FILE_WRITE);
    l0.toJson(doc);
    serializeJson(doc, fileL0);
    fileL0.close();

    if (SD.exists(L1_FILE))
    {
        SD.remove(L1_FILE);
    }
    File fileL1 = SD.open(L1_FILE, FILE_WRITE);
    l1.toJson(doc);
    serializeJson(doc, fileL1);
    fileL1.close();

    Serial.println("Current LED State saved on the SD card");
}

void reloadStates()
{
    // L0
    if (l0.enabled)
    {
        l0Value.setTargetValue(PWMRANGE - l0.level);
    }
    else
    {
        l0Value.setTargetValue(PWMRANGE);
    }

    // L1
    if (l1.enabled)
    {
        l1RValue.setTargetValue(PWMRANGE - l1.levelR);
        l1GValue.setTargetValue(PWMRANGE - l1.levelG);
        l1BValue.setTargetValue(PWMRANGE - l1.levelB);
    }
    else
    {
        l1RValue.setTargetValue(PWMRANGE);
        l1GValue.setTargetValue(PWMRANGE);
        l1BValue.setTargetValue(PWMRANGE);
    }

    Serial.println("GPIO state update");
}

void updateGPIO()
{
    if (!l0Value.isFinished())
    {
        analogWrite(PIN_L0, l0Value.getCurrentValue());
    }
    if (!l1RValue.isFinished())
    {
        analogWrite(PIN_L1_R, l1RValue.getCurrentValue());
    }
    if (!l1GValue.isFinished())
    {
        analogWrite(PIN_L1_G, l1GValue.getCurrentValue());
    }
    if (!l1BValue.isFinished())
    {
        analogWrite(PIN_L1_B, l1BValue.getCurrentValue());
    }
}

void updateFromWeb()
{
    reloadStates();
    makeStateSaveAt = millis() + WAIT_FOR_STATE_SAVE;
}

void initSD()
{
    if (!SD.begin(PIN_CS))
    {
        Serial.println("Load SD card error!");
    }
}

void loadModelFromFile(JsonDocument *doc, const char *path)
{
    Serial.println(String("Loading file: ") + path);
    if (!SD.exists(path))
    {
        Serial.println("File doesn't exist");
        return;
    }
    File file = SD.open(path, FILE_READ);
    DeserializationError error = deserializeJson(*doc, file);
    file.close();
    if (error)
    {
        Serial.print("Failed to read file: ");
        Serial.println(path);
        Serial.print("JSON Error: ");
        Serial.println(error.c_str());
        return;
    }
}

void loadSettings()
{
    StaticJsonDocument<800> doc;
    loadModelFromFile(&doc, SETTINGS_FILE);
    settings.fromJson(doc);
    loadModelFromFile(&doc, L0_FILE);
    l0.fromJson(doc);
    loadModelFromFile(&doc, L1_FILE);
    l1.fromJson(doc);
}

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    for (auto entry : settings.APlist)
    {
        wifiMulti.addAP(entry->ssid, entry->password);
    }

    wifiMulti.run();
}

void initWebApp()
{
    WebAppBegin(80);
    WebAppSetL0Model(&l0);
    WebAppSetL1Model(&l1);
    WebAppOnAPIUpdate(updateFromWeb);
}

void initIO()
{
    pinMode(PIN_L0, OUTPUT);
    pinMode(PIN_L1_R, OUTPUT);
    pinMode(PIN_L1_G, OUTPUT);
    pinMode(PIN_L1_B, OUTPUT);
    digitalWrite(PIN_L0, HIGH);
    digitalWrite(PIN_L1_R, HIGH);
    digitalWrite(PIN_L1_G, HIGH);
    digitalWrite(PIN_L1_B, HIGH);
}

void initDDNS()
{
    HTTPClient http;
    String url = String(settings.DDNSInitUrl) + WiFi.localIP().toString();

    if (!http.begin(url))
    { // HTTP
        Serial.println("init DDNS error");
        return;
    }
    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println(payload);
    Serial.println("init DDNS result: " + payload + " code: " + httpCode);

    http.end();
}

long longDate(String date)
{
    String tmp = String(date);
    tmp.replace(":", "");
    return tmp.toInt();
}

void initTimeJob()
{
    timeClient = new NTPClient(ntpUDP, settings.ntpHost, settings.utcOffset);
    timeClient->begin();
}

void checkNightMode()
{
    long now = longDate(timeClient->getFormattedTime());
    long turnOnDate = longDate(settings.turn_on_L0);
    long turnOffDate = longDate(settings.turn_off_L0);
    bool nightModeNow = false;
    if ((turnOnDate < turnOffDate) && ((now >= turnOffDate) || (now < turnOnDate)))
    { // Contains midnight
        nightModeNow = true;
    }
    if ((turnOnDate > turnOffDate) && (now >= turnOffDate) && (now < turnOnDate))
    {
        nightModeNow = true;
    }

    if (nightModeNow != nightMode || !timeJobInited)
    {
        timeJobInited = true;
        nightMode = nightModeNow;
        Serial.println(String("night mode status changed; night mode: ") + nightMode);
        Serial.print("now: ");
        Serial.println(now);
        Serial.print("turnOffDate: ");
        Serial.println(turnOffDate);
        Serial.print("turnOnDate: ");
        Serial.println(turnOnDate);

        l0.enabled = !nightMode;
        reloadStates();
    }
}

void setup()
{
    Serial.begin(SERIAL_SPEED);
    //Serial.setDebugOutput(true);
    Serial.print("Hello!\n");

    initIO();
    initSD();
    loadSettings();
    reloadStates();
    initWiFi();
    initWebApp();
    initTimeJob();
}

void saveStatesToSD()
{
    if (makeStateSaveAt && millis() > makeStateSaveAt)
    {
        makeStateSave();
        makeStateSaveAt = 0;
    }
}

int16_t t = 0;

void loop()
{
    updateGPIO();

    if (wifiMulti.run() != WL_CONNECTED)
    {
        networkActionsInited = false;
        if (t++ <= 0)
        {
            Serial.println("no WiFi...");
            t = 1;
        }
        //delay(1000);
        return;
    }
    else if (!networkActionsInited)
    {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        initDDNS();
        networkActionsInited = true;
    }

    WebAppLoop();
    timeClient->update();
    checkNightMode();
    saveStatesToSD();
}