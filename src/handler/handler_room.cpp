#include <handler/handler_room.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "spiff_manager.h"
#include "relay_control.h"
#include "helper.h"
#include "web_sockets.h"
#include "server_manager.h"


void handlerRooms(){
  handlerRoom();
  hanlderSetChannels();
}

static void handlerRoom(){
  wsEvents.on("get-room", [](JsonVariant data, AsyncWebSocketClient *client) {
    int roomId = data["room"];
    Serial.printf("Client %u requested room %d\n", client->id(), roomId);

    if (!LittleFS.exists("/database.json")) {
      Serial.println("File database.json tidak ditemukan");
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docDBRooms;

    if (!loadJsonFromFile("/database.json", docDBRooms)) {
      Serial.println("Gagal membaca database.json");
      client->text(makeJsonMessageWS("error", "Gagal membaca database"));
      return;
    }

    JsonArray arrRooms = docDBRooms["rooms"].as<JsonArray>();
    JsonObject foundRoom;
    for (JsonObject room : arrRooms) {
      if (room["id"] == String(roomId)) {
        foundRoom = room;
        break;
      }
    }

    if (foundRoom.isNull()) {
      Serial.printf("Ruangan dengan ID %d tidak ditemukan\n", roomId);

      client->text(makeJsonMessageWS("error", "Ruangan tidak ditemukan"));
      return;
    }

    Serial.println(makeJsonMessageWS("room-data", "test message hello world"));

    Serial.printf("Sending", foundRoom["name"].as<const char*>(), "data to client %u\n", client->id());
    client->text(makeJsonDataWS("room-data", foundRoom) );
  });
}

static void hanlderSetChannels(){
  AsyncCallbackJsonWebHandler *handlerSetChannel = new AsyncCallbackJsonWebHandler("/api/channel", [](AsyncWebServerRequest *request, JsonVariant &json){
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