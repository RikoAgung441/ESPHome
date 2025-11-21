#include <handler/handler_setting.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "helper.h"
#include "web_sockets.h"
#include "server_manager.h"


void handlerSettings(){
  handlerSetting();
  handlerSetSetting();
}

static void handlerSetting(){
  wsEvents.on("get-settings", [](JsonVariant data, AsyncWebSocketClient *client) {
      Serial.printf("Client %u requested settings\n", client->id());
  
      if (!LittleFS.exists("/database.json")) {
        Serial.println("File database.json tidak ditemukan");
        client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
        return;
      }
  
      JsonDocument docSettings;
  
      if (!loadJsonFromFile("/database.json", docSettings)) {
        Serial.println("Gagal membaca database.json");
        client->text(makeJsonMessageWS("error", "Gagal membaca settings"));
        return;
      }
  
      JsonObject settingObj = docSettings["settings"].as<JsonObject>();
  
      client->text(makeJsonDataWS("settings", settingObj) );
    });
}

static void handlerSetSetting(){
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