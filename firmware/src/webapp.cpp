#include "webapp.h"

#include <ESP8266WebServer.h>
#include <SPI.h>
#include <SD.h>

static void handleNotFound();
static void handleAPI_L0_GET();
static void handleAPI_L0_SET();
static void handleAPI_L1_GET();
static void handleAPI_L1_SET();

static ESP8266WebServer* server = nullptr;

static L0Model *L0ModelPtr = nullptr;
static L1Model *L1ModelPtr = nullptr;
static std::function<void(void)> onAPIUpdate = nullptr;

void WebAppBegin(int port){
    if(server != nullptr){
      return;
    }
    server = new ESP8266WebServer(port);

    server->on("/api/L0", HTTP_GET, handleAPI_L0_GET);
    server->on("/api/L0", HTTP_POST, handleAPI_L0_SET);
    server->on("/api/L1", HTTP_GET, handleAPI_L1_GET);
    server->on("/api/L1", HTTP_POST, handleAPI_L1_SET);

    server->on("/api/L0", HTTP_OPTIONS, []() {
      server->sendHeader("Access-Control-Allow-Origin", "*");
      server->sendHeader("Access-Control-Max-Age", "10000");
      server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
      server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
      server->send(200, "text/plain", "" );
    });
    server->on("/api/L1", HTTP_OPTIONS, []() {
      server->sendHeader("Access-Control-Allow-Origin", "*");
      server->sendHeader("Access-Control-Max-Age", "10000");
      server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
      server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
      server->send(200, "text/plain", "" );
    });







    server->onNotFound(handleNotFound);
    server->begin();
}

void WebAppLoop(){
  if(server == nullptr){
    return;
  }
  server->handleClient();
}

void WebAppEnd(){
  if(server == nullptr){
    return;
  }
  delete server;
  server = nullptr;
}

void WebAppSetL0Model(L0Model *model){
    L0ModelPtr = model;
}
void WebAppSetL1Model(L1Model *model){
    L1ModelPtr = model;
}

void WebAppOnAPIUpdate(std::function<void(void)> cb){
  onAPIUpdate = cb;
}

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  path = "webapp" + path;
  if (path.endsWith("/")) {
    path += "index.html";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm") || path.endsWith(".html")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (server->streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

// handlers
static void handleNotFound() {
  if (loadFromSdCard(server->uri())) {
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

static void internalError(){
    server->send(500, "text/plain", "Internal error");
}

static void handleAPI_L0_GET(){
    if(L0ModelPtr == nullptr){
        internalError();
        return; 
    }
    String out;
    L0ModelPtr->serialize(out);
    server->sendHeader("Access-Control-Allow-Origin", "*");
    server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server->send(200, "application/json", out);
}

static void handleAPI_L0_SET(){
    if(L0ModelPtr == nullptr){
        internalError();
        return; 
    }
    String body = server->arg("plain");
    L0ModelPtr->deserialize(body);
    if(onAPIUpdate != nullptr){
      onAPIUpdate();
    }
    handleAPI_L0_GET();
}

static void handleAPI_L1_GET(){
    if(L1ModelPtr == nullptr){
        internalError();
        return; 
    }
    String out;
    L1ModelPtr->serialize(out);
    server->sendHeader("Access-Control-Allow-Origin", "*");
    server->sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server->sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    server->send(200, "application/json", out);
}

static void handleAPI_L1_SET(){
    if(L1ModelPtr == nullptr){
        internalError();
        return;
    }
    String body = server->arg("plain");
    L1ModelPtr->deserialize(body);
    if(onAPIUpdate != nullptr){
      onAPIUpdate();
    }
    handleAPI_L1_GET();
}
