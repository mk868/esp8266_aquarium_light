#include "SDWebApp.h"

#include <ESP8266WebServer.h>
#include <SPI.h>
#include <SD.h>

static void handleNotFound();

static ESP8266WebServer *server = nullptr;

static const char *DEFAULT_FILES_IN_DIR[] = {"index.htm", "index.txt"}; //FAT file names...
static const char *WEBAPP_DIR = "";
static const char *DataTypes[] = {
    ".htm", "text/html",
    ".html", "text/html",
    ".css", "text/css",
    ".js", "application/javascript",
    ".png", "image/png",
    ".gif", "image/gif",
    ".jpg", "image/jpeg",
    ".ico", "image/x-icon",
    ".xml", "text/xml",
    ".pdf", "application/pdf",
    ".zip", "application/zip",
    "text/plain" //default
};

void SDWebAppBegin(ESP8266WebServer *webServer, const char *staticContentDir)
{
  WEBAPP_DIR = staticContentDir;
  server = webServer;

  server->onNotFound(handleNotFound);
}

bool loadFromSdCard(String path)
{
  String dataType = DataTypes[sizeof(DataTypes) - 1];
  path = WEBAPP_DIR + path;

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory())
  {
    for (uint i = 0; i < sizeof(DEFAULT_FILES_IN_DIR); i++)
    {
      auto fileName = DEFAULT_FILES_IN_DIR[i];
      if (!SD.exists(path + fileName))
      {
        continue;
      }
      path += fileName;
      break;
    }
    dataFile.close();
    dataFile = SD.open(path.c_str());
  }

  //TODO: directory listing
  for (uint i = 0; i < sizeof(DataTypes) - 1; i += 2)
  {
    if (!path.endsWith(DataTypes[i]))
    {
      continue;
    }
    dataType = DataTypes[i + 1];
    break;
  }

  if (!dataFile)
  {
    Serial.println(String("Path: ") + path + " not found");
    dataFile.close();
    return false;
  }

  if (server->streamFile(dataFile, dataType) != dataFile.size())
  {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

// handlers
static void handleNotFound()
{
  if (loadFromSdCard(server->uri()))
  {
    return;
  }
  String message = "ERROR 404\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\n";
  server->send(404, "text/plain", message);
  Serial.print(message);
}
