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

  server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "{}");
      return;
    }

    JsonDocument docDBSetting;

    if (!loadJsonFromFile("/database.json", docDBSetting)) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca database\"}");
      return;
    }

    JsonObject settingObj = docDBSetting["settings"].as<JsonObject>();
    String resJson;
    serializeJson(settingObj, resJson);
    // file.close();

    request->send(200, "application/json", resJson);
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
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca database\"}");
      return;
    }

    JsonArray arr = docDB["rooms"].as<JsonArray>();
    String resJson;
    serializeJson(arr, resJson);
    request->send(200, "application/json", resJson);
  });

  server.on("/api/room", HTTP_GET, [](AsyncWebServerRequest *request){
    String reqRoom = "";
    Serial.println("Request room received");

    if (request->hasParam("id")) {
      Serial.println("Parameter id ditemukan");
      reqRoom = request->getParam("id")->value();
      Serial.println("ID: " + reqRoom);
    }

    if(reqRoom.isEmpty()) {
      request->send(400, "application/json", "{\"msg\":\"Parameter room diperlukan\"}");
      return;
    }

    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "[]");
      return;
    }

    JsonDocument docDBRooms;

    if (!loadJsonFromFile("/database.json", docDBRooms)) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca database\"}");
      return;
    }

    // Serial.println("Data rooms:");
    // Serial.println(docDBRooms["rooms"].as<JsonArray>());
    // Serial.println("Full JSON:");
    // Serial.println(docDBRooms.as<JsonArray>());

    JsonArray arr = docDBRooms["rooms"].as<JsonArray>();
    Serial.println(arr);
    JsonObject foundRoom;
    for (JsonObject room : arr) {
      if (room["id"] == reqRoom) {
        Serial.println("Ruangan ditemukan: " + reqRoom);
        Serial.println(room["name"].as<String>());
        foundRoom = room;
        break;
      }
    }

    if (foundRoom.isNull()) {
      request->send(404, "application/json", "{\"msg\":\"Ruangan tidak ditemukan\"}");
      return;
    }

    String resJson;
    serializeJson(foundRoom, resJson);
    request->send(200, "application/json", resJson);
    return;
  });
}
