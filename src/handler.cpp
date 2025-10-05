#include <server_manager.h>
#include <LittleFS.h>
#include <FS.h>
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "relay_control.h"
#include "helper.h"
#include "web_sockets.h"


void handlerSetSettings(){
   AsyncCallbackJsonWebHandler *handlerSetSettings = new AsyncCallbackJsonWebHandler("/api/set", [](AsyncWebServerRequest *request, JsonVariant &json) {

    if (!json.is<JsonObject>()) {
      Serial.println("Data tidak valid");
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    String key = reqObj["key"];
    String value = reqObj["value"];

    if (key.isEmpty() || value.isEmpty()) {
      Serial.println("Data tidak valid 2");
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    if(key.length() > 15 || value.length() > 50) {
      Serial.println("Data length tidak valid");
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    const char* allowedKeys[] = {"ssid", "password", "name"};
    bool allowed = false;
    for (const char* k : allowedKeys) {
      if (key.equals(k)) {
        allowed = true;
        break;
      }
    }

    if (!allowed) {
      Serial.println("Data tidak diizinkan");
      request->send(400, "application/json", makeJsonMessage("Data tidak diizinkan"));
      return;
    }


    JsonDocument docDB;

    if (!loadJsonFromFile("/database.json", docDB)) {
      request->send(500, "application/json", makeJsonMessage("Gagal membaca database"));
      return;
    }
    

    docDB["settings"][key] = value;

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
      return;
    }
    serializeJsonPretty(docDB, file);
    file.close();
    broadcast("settings-updated", docDB["settings"]);

    request->send(200, "application/json", makeJsonMessage("Data berhasil disimpan"));
  });

  server.addHandler(handlerSetSettings);
}

void handlerSwitchChannel(){
      AsyncCallbackJsonWebHandler *handlerSetChannel = new AsyncCallbackJsonWebHandler("/api/channel", [](AsyncWebServerRequest *request, JsonVariant &json) {
      Serial.println("Received /api/channel request");

    if( !json.is<JsonObject>()) {
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    int reqRoomId = reqObj["room"];
    JsonArray reqChannels = reqObj["channels"].as<JsonArray>();

    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "[]");
      return;
    }

    JsonDocument docDBRooms;

    if (!loadJsonFromFile("/database.json", docDBRooms)) {
      request->send(500, "application/json", makeJsonMessage("Gagal membaca database"));
      return;
    }

    JsonArray arr = docDBRooms["rooms"].as<JsonArray>();
    JsonObject foundRoom;
    for (JsonObject room : arr) {
      if (room["id"] == String(reqRoomId)) {
        foundRoom = room;
        break;
      }
    }

    if (foundRoom.isNull()) {
      request->send(404, "application/json", makeJsonMessage("Ruangan tidak ditemukan"));
      return;
    }

    JsonArray targetChannels = foundRoom["channels"].as<JsonArray>();
    
    for (JsonObject channel : reqChannels) {
      for (JsonObject targetChannel : targetChannels) {
        if (channel["id"] == targetChannel["id"]) {
          targetChannel["status"] = channel["status"];
          int pin = targetChannel["pin"];
          relaySwitch(pin, channel["status"].as<bool>());
          break;
        }
      }
    }


    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
      return;
    }
    serializeJson(docDBRooms, file);
    file.close();

    broadcast("room-data", foundRoom);

    request->send(200, "application/json", makeJsonMessage("Data berhasil disimpan"));
  });
  server.addHandler(handlerSetChannel);
}
