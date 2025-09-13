#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
// #include "preferences_manager.h"

AsyncWebServer server(80);
String resJson;

void webServerInit() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS gagal mount!");
  }else{
    Serial.println("SPIFFS mounted successfully.");
    listSPIFFSFiles();
  }

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

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

void sendJSON(AsyncWebServerRequest *request, int code, const String &json) {
  request->send(code, "application/json", json);
}

static void endpointSetting() {

  server.on("/setting", HTTP_GET, [](AsyncWebServerRequest *request){
    if (SPIFFS.exists("/setting.html")) {
      request->send(SPIFFS, "/setting.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: file tidak ditemukan di SPIFFS!");
    }
  });

  server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!SPIFFS.exists("/settings.json")) {
      request->send(200, "application/json", "{}");
      return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    resJson = file.readString();
    file.close();

    request->send(200, "application/json", resJson);
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/api/set", [](AsyncWebServerRequest *request, JsonVariant &json) {

    

    if (!json.is<JsonObject>()) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    JsonObject obj = json.as<JsonObject>();
    String key = obj["key"];
    String value = obj["value"];

    if (key.isEmpty() || value.isEmpty()) {
      request->send(400, "application/json", "{\"msg\":\"Key atau value tidak boleh kosong\"}");
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
      request->send(400, "application/json", "{\"msg\":\"Key tidak diperbolehkan\"}");
      return;
    }

    JsonDocument doc;
    File file = SPIFFS.open("/settings.json", "r");
    if (file) {
      DeserializationError err = deserializeJson(doc, file);
      file.close();
      if (err) {
        Serial.println("JSON lama rusak, akan direset.");
        doc.clear();
      }
    }

    doc[key] = value;

    file = SPIFFS.open("/settings.json", "w");
    if (!file) {
      request->send(500, "application/json","{\"msg\":\"Gagal menulis file\"}");
      return;
    }
    serializeJsonPretty(doc, file);
    file.close();


    JsonDocument res;
    res["status"] = "success";
    res["msg"] = "Data berhasil disimpan";

    serializeJson(res, resJson);
    request->send(200, "application/json", resJson);
  });

  server.addHandler(handler);
}

static void endpointRooms() {
  server.on("/rooms", HTTP_GET, [](AsyncWebServerRequest *request){
    if (SPIFFS.exists("/rooms.html")) {
      request->send(SPIFFS, "/rooms.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: file tidak ditemukan di SPIFFS!");
    }
  });
}
