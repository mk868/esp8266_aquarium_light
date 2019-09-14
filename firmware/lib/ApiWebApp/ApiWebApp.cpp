#include "ApiWebApp.h"

#include <Arduino.h>

static void handleApiGET();
static void handleApiPOST();

static ESP8266WebServer *server = nullptr;

struct ActionEntry
{
  const char *path;
  SerializableModel *model;
  std::function<void(void)> afterUpdate;
};

static std::vector<struct ActionEntry> Actions;

void ApiWebAppBegin(ESP8266WebServer *webServer)
{
  server = webServer;
}

void ApiWebAppAddModelApi(const char *path, SerializableModel *model, std::function<void(void)> afterUpdate)
{
  server->on(path, HTTP_GET, handleApiGET);
  server->on(path, HTTP_POST, handleApiPOST);

  struct ActionEntry action;
  action.path = path;
  action.model = model;
  action.afterUpdate = afterUpdate;
  Actions.push_back(action);
}

static void handleApiGET()
{
  Serial.println("GET " + server->uri());

  for (auto action : Actions)
  {
    if (server->uri() != action.path)
    {
      continue;
    }

    StaticJsonDocument<500> doc;
    action.model->toJson(doc);
    String out;
    serializeJson(doc, out);
    server->send(200, "application/json", out);

    return;
  }

  server->send(500, "text/plain", "Internal error");
  return;
}

static void handleApiPOST()
{
  Serial.println("POST " + server->uri());

  for (auto action : Actions)
  {
    if (server->uri() != action.path)
    {
      continue;
    }

    String body = server->arg("plain");
    StaticJsonDocument<500> doc;

    deserializeJson(doc, body);
    action.model->fromJson(doc);
    if (action.afterUpdate != nullptr)
    {
      action.afterUpdate();
    }

    action.model->toJson(doc);
    String out;
    serializeJson(doc, out);
    server->send(200, "application/json", out);

    return;
  }

  server->send(500, "text/plain", "Internal error");
  return;
}