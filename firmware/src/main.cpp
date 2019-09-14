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

#include "models/L0Model.h"
#include "models/L1Model.h"
#include "models/SettingsModel.h"

#include "SDSerializer.h"

#include "SmoothTransition.h"
#include "SDWebApp.h"
#include "ApiWebApp.h"

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer webServer;

L0Model l0DayMode;
L1Model l1DayMode;
L0Model l0NightMode;
L1Model l1NightMode;
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
    SDSerializer sdSerializer;
    sdSerializer.save(L0_FILE, &l0DayMode);
    sdSerializer.save(L1_FILE, &l1DayMode);
    sdSerializer.save(L0_NIGHT_FILE, &l0NightMode);
    sdSerializer.save(L1_NIGHT_FILE, &l1NightMode);

    Serial.println("Current LED State saved on the SD card");
}

void reloadStates()
{
    L0Model *l0Model = nightMode ? &l0NightMode : &l0DayMode;
    L1Model *l1Model = nightMode ? &l1NightMode : &l1DayMode;

    // L0
    if (l0Model->enabled)
    {
        l0Value.setTargetValue(PWMRANGE - l0Model->level);
    }
    else
    {
        l0Value.setTargetValue(PWMRANGE);
    }

    // L1
    if (l1Model->enabled)
    {
        l1RValue.setTargetValue(PWMRANGE - l1Model->levelR);
        l1GValue.setTargetValue(PWMRANGE - l1Model->levelG);
        l1BValue.setTargetValue(PWMRANGE - l1Model->levelB);
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

void loadSettings()
{
    SDSerializer sdSerializer(800);
    sdSerializer.load(SETTINGS_FILE, &settings);
    sdSerializer.load(L0_FILE, &l0DayMode);
    sdSerializer.load(L1_FILE, &l1DayMode);
    sdSerializer.load(L0_NIGHT_FILE, &l0NightMode);
    sdSerializer.load(L1_NIGHT_FILE, &l1NightMode);
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
    ApiWebAppBegin(&webServer);
    ApiWebAppAddModelApi("/api/L0", &l0DayMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/L1", &l1DayMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/L0.N", &l0NightMode, updateFromWeb);
    ApiWebAppAddModelApi("/api/L1.N", &l1NightMode, updateFromWeb);
    SDWebAppBegin(&webServer, "webapp");
    webServer.begin(80);
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
    webServer.handleClient();

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

    timeClient->update();
    checkNightMode();
    saveStatesToSD();
}