#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <SD.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <ArduinoJson.h>

#include "config.h"
#include "webapp.h"

#include "models/L0Model.h"
#include "models/L1Model.h"
#include "models/SettingsModel.h"

L0Model l0;
L1Model l1;
SettingsModel settings;

WiFiUDP ntpUDP;
NTPClient *timeClient;

bool nightMode;
bool timeJobInited = false;
unsigned long makeStateSaveAt = 0;

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

    Serial.println("Save state to SD...");
}

void updateGPIOState()
{
    // L0
    if (l0.enabled)
    {
        analogWrite(PIN_L0, PWMRANGE - l0.level);
    }
    else
    {
        digitalWrite(PIN_L0, HIGH);
    }

    // L1
    if (l1.enabled)
    {
        analogWrite(PIN_L1_R, PWMRANGE - l1.levelR);
        analogWrite(PIN_L1_G, PWMRANGE - l1.levelG);
        analogWrite(PIN_L1_B, PWMRANGE - l1.levelB);
    }
    else
    {
        digitalWrite(PIN_L1_R, HIGH);
        digitalWrite(PIN_L1_G, HIGH);
        digitalWrite(PIN_L1_B, HIGH);
    }

    Serial.println("GPIO state update");
}

void updateFromWeb()
{
    updateGPIOState();
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
    Serial.println(String("File exist: ") + SD.exists(path));
    File file = SD.open(path, FILE_READ);
    DeserializationError error = deserializeJson(*doc, file);
    file.close();
    if (error)
    {
        Serial.print("Failed to read file: ");
        Serial.println(path);
        return;
    }
}

void loadSettings()
{
    StaticJsonDocument<500> doc;
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
    WiFi.begin(settings.wifi_ssid, settings.wifi_password);
    Serial.print("Connecting to ");
    Serial.println(settings.wifi_ssid);

    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
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
        updateGPIOState();
    }
}

void setup()
{
    Serial.begin(SERIAL_SPEED);
    //Serial.setDebugOutput(true);
    Serial.print("Hello!\n");

    initSD();
    loadSettings();
    //TODO load state
    initIO();
    updateGPIOState();
    initWiFi();
    initWebApp();
    initTimeJob();
}

void loop()
{
    WebAppLoop();
    timeClient->update();
    checkNightMode();
    if (makeStateSaveAt && millis() > makeStateSaveAt)
    {
        makeStateSave();
        makeStateSaveAt = 0;
    }
}