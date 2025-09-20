#include <server_manager.h>
#include <LittleFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "relay_control.h"
// #include "preferences_manager.h"

AsyncWebServer server(80);
String resJson;

void webServerInit() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS gagal mount!");
  }else{
    Serial.println("LittleFS mounted successfully.");
    listSPIFFSFiles();
  }

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

void sendJSON(AsyncWebServerRequest *request, int code, const String &json) {
  request->send(code, "application/json", json);
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

    File file = LittleFS.open("/database.json", "r");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    // resJson = file.readString();
    JsonDocument docDBSetting;
    DeserializationError err = deserializeJson(docDBSetting, file);
    if (err) {
      Serial.print("❌ Gagal parse JSON: ");
      Serial.println(err.c_str());
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    JsonObject settingObj = docDBSetting["settings"].as<JsonObject>();
    serializeJson(settingObj, resJson);
    file.close();

    request->send(200, "application/json", resJson);
  });


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
    File file = LittleFS.open("/database.json", "r");

    if (file) {
      DeserializationError error = deserializeJson(docSet, file);
      file.close();

      if (error) {
        Serial.print("❌ Gagal parse JSON: ");
        Serial.println(error.c_str());
        request->send(500, "application/json", "{\"msg\":\"Gagal membaca database\"}");
        return;
      }
    } else {
      request->send(500, "application/json", "{\"msg\":\"File database.json tidak ditemukan\"}");
      return;
    }


    // if (!docSet.containsKey("settings")) {
    //   docSet["settings"] = JsonObject();
    // }

    docSet["settings"][key] = value;

    file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal menulis file\"}");
      return;
    }
    serializeJsonPretty(docSet, file);
    file.close();


    JsonDocument res;
    res["status"] = "success";
    res["msg"] = "Data berhasil disimpan";

    serializeJson(res, resJson);
    request->send(200, "application/json", resJson);
  });

  server.addHandler(handlerSetSettings);
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

    File file = LittleFS.open("/database.json", "r");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    resJson = file.readString();
    file.close();

    JsonDocument docDB;
    DeserializationError err = deserializeJson(docDB, resJson);
    if (err) {
      request->send(500, "application/json", "{\"msg\":\"Gagal memproses data\"}");
      return;
    }

    JsonArray arr = docDB["rooms"].as<JsonArray>();
    String roomsJson;
    serializeJson(arr, roomsJson);

    request->send(200, "application/json", roomsJson);
  });

  server.on("/api/room", HTTP_GET, [](AsyncWebServerRequest *request){
    String reqRoom = "";
    Serial.println("Request room received");

    // if (request->hasParam("id", true)) {
    //   Serial.println("Parameter id ditemukan");
    //   reqRoom = request->getParam("id", true)->value();
    //   Serial.print("Request room: ");
    //   Serial.println(reqRoom);
    // }

    if (request->hasParam("id")) {
      Serial.println("Parameter id ditemukan");
      reqRoom = request->getParam("id")->value();
      Serial.println("ID: " + reqRoom);
    }

    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "[]");
      return;
    }

    File file = LittleFS.open("/database.json", "r");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    resJson = file.readString();
    // Serial.println("Data dari file:");
    // Serial.println(resJson);
    file.close();

    if(reqRoom.isEmpty()) {
      request->send(400, "application/json", "{\"msg\":\"Parameter room diperlukan\"}");
      return;
    }

    JsonDocument docDBRooms;
    DeserializationError err = deserializeJson(docDBRooms, resJson);
    if (err) {
      request->send(500, "application/json", "{\"msg\":\"Gagal memproses data\"}");
      return;
    }

    Serial.println("Data rooms:");
    Serial.println(docDBRooms["rooms"].as<JsonArray>());
    Serial.println("Full JSON:");
    Serial.println(docDBRooms.as<JsonArray>());

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

    String singleRoomJson;
    serializeJson(foundRoom, singleRoomJson);
    request->send(200, "application/json", singleRoomJson);
    return;
  });

  AsyncCallbackJsonWebHandler *handlerSetChannel = new AsyncCallbackJsonWebHandler("/api/channel", [](AsyncWebServerRequest *request, JsonVariant &json) {

    if( !json.is<JsonObject>()) {
      request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    int reqRoomId = reqObj["room"];
    JsonArray reqChannels = reqObj["channels"].as<JsonArray>();

    Serial.println("Request channel received");
    Serial.print("Room ID: "); Serial.println(reqRoomId);
    Serial.print("Channels: "); Serial.println(reqChannels.size());
    // Serial.println(reqChannels[]);

    // if (reqRoomId == 0 || reqChannels.isNull() || reqChannels.size() == 0) {
    //   request->send(400, "application/json", "{\"msg\":\"Data tidak valid\"}");
    //   return;
    // }

    if (!LittleFS.exists("/database.json")) {
      request->send(200, "application/json", "[]");
      return;
    }

    File file = LittleFS.open("/database.json", "r");
    if (!file) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    resJson = file.readString();
    // Serial.println("Data dari file:");
    // Serial.println(resJson);
    file.close();


    JsonDocument docDBRooms;
    DeserializationError err = deserializeJson(docDBRooms, resJson);
    if (err) {
      request->send(500, "application/json", "{\"msg\":\"Gagal memproses data\"}");
      return;
    }

    // Serial.println("Data rooms:");
    // Serial.println(docDBRooms["rooms"].as<JsonArray>());
    // Serial.println("Full JSON:");
    // Serial.println(docDBRooms.as<JsonArray>());

    JsonArray arr = docDBRooms["rooms"].as<JsonArray>();
    // Serial.println(arr);
    JsonObject foundRoom;
    for (JsonObject room : arr) {
      if (room["id"] == String(reqRoomId)) {
        // Serial.println("Ruangan ditemukan: " + String(reqRoomId));
        // Serial.println(room["name"].as<String>());
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

    serializeJson(docDBRooms, resJson);

    File file2 = LittleFS.open("/database.json", "w");
    if (!file2) {
      request->send(500, "application/json", "{\"msg\":\"Gagal membaca file\"}");
      return;
    }

    file2.print(resJson);
    file2.close();

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
      // relaySwitch(channel["id"].as<int>(), channel["status"].as<bool>());
    }



    request->send(200, "application/json", "{\"msg\":\"Data berhasil disimpan\"}");
  });
  server.addHandler(handlerSetChannel);
}
