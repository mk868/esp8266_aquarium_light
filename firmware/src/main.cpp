#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <SD.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>
#include <RTClib.h>

#include "config.h"

#include "models/L0Model.h"
#include "models/L1Model.h"
#include "models/CoreSettingsModel.h"
#include "models/SettingsModel.h"

#include "SDSerializer.h"

#include "SmoothTransition.h"
#include "SDWebApp.h"
#include "ApiWebApp.h"

RTC_DS3231 rtc;

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer webServer;

L0Model l0DayMode;
L1Model l1DayMode;
L0Model l0NightMode;
L1Model l1NightMode;
SettingsModel settings;         // read only
CoreSettingsModel coreSettings; // read only

SmoothTransition l0Value;
SmoothTransition l1RValue;
SmoothTransition l1GValue;
SmoothTransition l1BValue;

bool nightMode = true; // by default enabled
unsigned long makeStateSaveAt = 0;

bool networkActionsInited = false;

void makeStateSave()
{
    SDSerializer sdSerializer;
    sdSerializer.save(SETTINGS_FILE, &settings);
    sdSerializer.save(DAY_L0_FILE, &l0DayMode);
    sdSerializer.save(DAY_L1_FILE, &l1DayMode);
    sdSerializer.save(NIGHT_L0_FILE, &l0NightMode);
    sdSerializer.save(NIGHT_L1_FILE, &l1NightMode);

    Serial.println("Current LED State saved on the SD card");
}

void reloadStates()
{
    L0Model *l0Model = nightMode ? &l0NightMode : &l0DayMode;
    L1Model *l1Model = nightMode ? &l1NightMode : &l1DayMode;

    // L0
    if (l0Model->enabled)
    {
        l0Value.setValue(PWMRANGE - l0Model->level, ANIMATION_SPEED);
    }
    else
    {
        l0Value.setValue(PWMRANGE, ANIMATION_SPEED);
    }

    // L1
    if (l1Model->enabled)
    {
        l1RValue.setValue(PWMRANGE - l1Model->levelR, ANIMATION_SPEED);
        l1GValue.setValue(PWMRANGE - l1Model->levelG, ANIMATION_SPEED);
        l1BValue.setValue(PWMRANGE - l1Model->levelB, ANIMATION_SPEED);
    }
    else
    {
        l1RValue.setValue(PWMRANGE, ANIMATION_SPEED);
        l1GValue.setValue(PWMRANGE, ANIMATION_SPEED);
        l1BValue.setValue(PWMRANGE, ANIMATION_SPEED);
    }

    Serial.println("GPIO state update");
}

void updateGPIO()
{
    l0Value.update();
    l1RValue.update();
    l1GValue.update();
    l1BValue.update();
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
        while (true)
        {
        }
    }
}

void loadSettings()
{
    SDSerializer sdSerializer(800);
    sdSerializer.load(CORE_SETTINGS_FILE, &coreSettings);
    sdSerializer.load(SETTINGS_FILE, &settings);
    sdSerializer.load(DAY_L0_FILE, &l0DayMode);
    sdSerializer.load(DAY_L1_FILE, &l1DayMode);
    sdSerializer.load(NIGHT_L0_FILE, &l0NightMode);
    sdSerializer.load(NIGHT_L1_FILE, &l1NightMode);
}

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    for (auto entry : coreSettings.APlist)
    {
        wifiMulti.addAP(entry->ssid, entry->password);
    }

    wifiMulti.run();
}

void initWebApp()
{
    ApiWebAppBegin(&webServer);
    ApiWebAppAddModelApi("/api/day/L0", &l0DayMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/day/L1", &l1DayMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/night/L0", &l0NightMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/night/L1", &l1NightMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/settings", &settings, updateFromWeb);
    SDWebAppBegin(&webServer, "webapp");
    webServer.begin(80);
}

void initIO()
{
    l0Value.begin(PIN_L0, PWMRANGE);
    l1RValue.begin(PIN_L1_R, PWMRANGE);
    l1GValue.begin(PIN_L1_G, PWMRANGE);
    l1BValue.begin(PIN_L1_B, PWMRANGE);
}

void initDDNS()
{
    HTTPClient http;
    String url = String(coreSettings.DDNSInitUrl) + WiFi.localIP().toString();

    if (!http.begin(url))
    { // HTTP
        Serial.println("init DDNS error");
        return;
    }
    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println("init DDNS result: " + payload + " code: " + httpCode);

    http.end();
}

void initRTC()
{
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        while (true)
        {
        }
    }
}

long longDate(String date)
{
    String tmp = String(date);
    tmp.replace(":", "");
    if (tmp.length() < 5)
    {                // hh:mm
        tmp += "00"; // add :ss
    }
    return tmp.toInt();
}

void checkNightMode()
{
    //On
    if (settings.night_mode_enabled == On)
    {
        if (!nightMode)
        {
            nightMode = true;
            Serial.println("[settings] night mode = true");

            reloadStates();
        }
        return;
    }

    //Off
    if (settings.night_mode_enabled == Off)
    {
        if (nightMode)
        {
            nightMode = false;
            Serial.println("[settings] night mode = false");

            reloadStates();
        }
        return;
    }

    DateTime nowDate = rtc.now();

    //Auto
    int timezoneOffset = longDate(settings.timezone);
    long now = nowDate.hour() * 10000 + nowDate.minute() * 100 + nowDate.second() + timezoneOffset;
    long startTime = longDate(settings.night_mode_start);
    long endTime = longDate(settings.night_mode_end);
    bool nightModeNow = false;
    if ((startTime > endTime) && ((now <= endTime) || (now >= startTime)))
    { // Contains midnight
        nightModeNow = true;
    }
    if ((startTime < endTime) && (now <= endTime) && (now >= startTime))
    {
        nightModeNow = true;
    }

    if (nightModeNow != nightMode)
    {
        nightMode = nightModeNow;
        Serial.println(String("[auto] night mode = ") + nightMode);
        Serial.print(" now: ");
        Serial.print(now);
        Serial.print(" startTime: ");
        Serial.print(startTime);
        Serial.print(" endTime: ");
        Serial.println(endTime);

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
    initRTC();
}

void saveStatesToSD()
{
    if (makeStateSaveAt && millis() > makeStateSaveAt)
    {
        makeStateSave();
        makeStateSaveAt = 0;
    }
}

unsigned long lastTick = 0;

void slowLoop()
{
    if (wifiMulti.run() != WL_CONNECTED)
    {
        networkActionsInited = false;
        Serial.println("no WiFi...");
    }
    else if (!networkActionsInited)
    {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        initDDNS();
        networkActionsInited = true;
    }

    checkNightMode();
    saveStatesToSD();
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void loop()
{
    unsigned long milisNow = millis();

    if (
        ((milisNow - lastTick) > 500) ||
        (lastTick > milisNow)) // overflow
    {
        lastTick = milisNow;
        slowLoop();
    }
    updateGPIO();
    webServer.handleClient();
}