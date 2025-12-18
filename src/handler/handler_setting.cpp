#include <handler/handler_setting.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "helper.h"
#include <server/web_sockets.h>
#include <server/server_manager.h>
#include "connection_config.h"
#include "debug.h"


void handlerSettings(){
  handlerSetting();
  handlerSetSetting();
}

void handlerSetting(){
  wsEvents.on("get:settings", [](JsonVariant data, AsyncWebSocketClient *client) {
    LOG_INFO("Client %u requested settings", client->id());


    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docSettings;

    if (!loadJsonFromFile("/database.json", docSettings)) {
      LOG_ERROR("Gagal membaca database.json");
      client->text(makeJsonMessageWS("error", "Gagal membaca settings"));
      return;
    }

    JsonObject settingObj = docSettings["settings"].as<JsonObject>();

    client->text(makeJsonDataWS("settings:data", settingObj) );
  });

  server.on("/api/scan-network", HTTP_GET,
    [](AsyncWebServerRequest *request) {
  
      if (!isWifiScanRunning() && !isWifiScanReady()) {
        startWifiScan();
        request->send(200, "application/json",
          "{\"status\":\"scanning\"}");
        return;
      }
  
      if (isWifiScanRunning()) {
        request->send(200, "application/json",
          "{\"status\":\"scanning\"}");
        return;
      }
  
      String json;
      serializeJson(getWifiScanResult(), json);
      request->send(200, "application/json", json);
      resetWifiScan();
      return;
  });
}

void handlerSetSetting() {
  server.addHandler(
    new AsyncCallbackJsonWebHandler("/api/settings/save",
      [](AsyncWebServerRequest *request, JsonVariant &json) {

        if (!json.is<JsonObject>()) {
          request->send(400, "application/json", makeJsonMessage("Invalid JSON"));
          return;
        }

        JsonObject req = json.as<JsonObject>();

        // Load database
        if (!LittleFS.exists("/database.json")) {
          request->send(500, "application/json", makeJsonMessage("Database not found"));
          return;
        }

        JsonDocument doc;
        if (!loadJsonFromFile("/database.json", doc)) {
          request->send(500, "application/json", makeJsonMessage("Failed to read database"));
          return;
        }

        JsonObject settings = doc["settings"].to<JsonObject>();

        // Assign values (safe)
        settings["ssidAp"]     = getString(req, "ssidAp");
        settings["passwordAp"] = getString(req, "passwordAp");
        settings["ssid"]       = getString(req, "ssid");
        settings["password"]   = getString(req, "password");
        settings["static_ip"]  = getString(req, "static_ip");
        settings["gateway"]    = getString(req, "gateway");
        settings["subnet"]     = getString(req, "subnet");
        settings["dns1"]       = getString(req, "dns1");
        settings["dns2"]       = getString(req, "dns2");
        settings["dhcp"]       = getBool(req, "dhcp", true);

        File file = LittleFS.open("/database.json", "w");
        if (!file) {
          request->send(500, "application/json", makeJsonMessage("Write failed"));
          return;
        }

        serializeJson(doc, file);
        file.close();

        // String settingsStr;
        // serializeJson(doc, settingsStr);
        // LOG_INFO("Settings saved: %s", settingsStr.c_str());


        broadcast("settings:data", settings);
        request->send(200, "application/json", makeJsonMessage("Settings saved"));
      }
    )
  );
}
