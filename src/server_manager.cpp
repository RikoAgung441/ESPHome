#include <server_manager.h>
#include <LittleFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "relay_control.h"
#include "helper.h"


AsyncWebServer server(80);

void webServerInit() {
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  endpointSetting();
  endpointRooms();

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, World!");
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not found");
  });

  server.begin();
  Serial.println("Web server started!");
  
}

static void endpointSetting() {

  server.on("/setting", HTTP_GET, [](AsyncWebServerRequest *request){
    if (LittleFS.exists("/setting.html")) {
      request->send(LittleFS, "/setting.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: file tidak ditemukan di LittleFS!");
    }
  });
}

static void endpointRooms() {
  server.on("/room", HTTP_GET, [](AsyncWebServerRequest *request){
    if (LittleFS.exists("/room.html")) {
      request->send(LittleFS, "/room.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: file tidak ditemukan di LittleFS!");
    }
  });

  server.on("/api/rooms", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "[]");
      return;
    }
    
    JsonDocument docDB;

    if (!loadJsonFromFile("/database.json", docDB)) {
      request->send(500, "application/json", makeJsonMessage("Gagal membaca database"));
      return;
    }

    JsonArray arr = docDB["rooms"].as<JsonArray>();
    String resJson;
    serializeJson(arr, resJson);
    request->send(200, "application/json", resJson);
  });
}
