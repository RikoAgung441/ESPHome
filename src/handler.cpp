#include <server_manager.h>
#include <LittleFS.h>
#include <FS.h>
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "relay_control.h"
#include "helper.h"


void handlerSetSettings(){
   AsyncCallbackJsonWebHandler *handlerSetSettings = new AsyncCallbackJsonWebHandler("/api/set", [](AsyncWebServerRequest *request, JsonVariant &json) {

    if (!json.is<JsonObject>()) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    String key = reqObj["key"];
    String value = reqObj["value"];

    if (key.isEmpty() || value.isEmpty()) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    if(key.length() > 15 || value.length() > 50) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    const char* allowedKeys[] = {"ssid", "password"};
    bool allowed = false;
    for (const char* k : allowedKeys) {
      if (key.equals(k)) {
        allowed = true;
        break;
      }
    }

    if (!allowed) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }


    JsonDocument docSet;

    if (!loadJsonFromFile("/database.json", docSet)) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca database\"}");
      return;
    }
    

    docSet["settings"][key] = value;

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal menulis file\"}");
      return;
    }
    serializeJsonPretty(docSet, file);
    file.close();


    JsonDocument resContent;
    resContent["status"] = "success";
    resContent["msg"] = "Data berhasil disimpan";

    String resJson;
    serializeJson(resContent, resJson);
    request->send(200, "application/json", resJson);
  });

  server.addHandler(handlerSetSettings);
}

void handlerSwitchChannel(){
   AsyncCallbackJsonWebHandler *handlerSetChannel = new AsyncCallbackJsonWebHandler("/api/channel", [](AsyncWebServerRequest *request, JsonVariant &json) {

    if( !json.is<JsonObject>()) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    int reqRoomId = reqObj["room"];
    JsonArray reqChannels = reqObj["channels"].as<JsonArray>();

    // Serial.println("Request channel received");
    // Serial.print("Room ID: "); Serial.println(reqRoomId);
    // Serial.print("Channels: "); Serial.println(reqChannels.size());

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
    JsonObject foundRoom;
    for (JsonObject room : arr) {
      if (room["id"] == String(reqRoomId)) {
        foundRoom = room;
        break;
      }
    }

    if (foundRoom.isNull()) {
      request->send(404, "application/json", "{\"msg\":\"Ruangan tidak ditemukan\"}");
      return;
    }

    JsonArray targetChannels = foundRoom["channels"].as<JsonArray>();
    for (JsonObject channel : reqChannels) {
      for (JsonObject targetChannel : targetChannels) {
        if (channel["id"] == targetChannel["id"]) {
          targetChannel["status"] = channel["status"];
          break;
        }
      }
    }


    String newFileContent;
    serializeJson(docDBRooms, newFileContent);

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    file.print(newFileContent);
    file.close();

    for (JsonObject channel : reqChannels) {
      Serial.print("Channel ID: "); Serial.print(channel["id"].as<int>());
      Serial.print(" - Status: "); Serial.println(channel["status"].as<bool>());
      for (JsonObject targetChannel : targetChannels) {
        if (channel["id"] == targetChannel["id"]) {
          int pin = targetChannel["pin"];
          Serial.print(" - Pin: "); Serial.println(pin);
          relaySwitch(pin, channel["status"].as<bool>());
          break;
        }
      }
    }

    request->send(200, "application/json", "{\"msg\":\"Data berhasil disimpan\"}");
  });
  server.addHandler(handlerSetChannel);
}
