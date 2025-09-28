#include "web_sockets.h"
#include <map>
#include "pzem_config.h"
#include <LittleFS.h>
#include "helper.h"

AsyncWebSocket ws("/ws");
EventEmitter wsEvents;

void broadcast(const String &eventName, JsonVariant data) {
  JsonDocument doc;
  doc["event"] = eventName;
  doc["data"] = data;

  String out;
  serializeJson(doc, out);
  ws.textAll(out);
}

static void onWsEvent(AsyncWebSocket *server,
                AsyncWebSocketClient *client,
                AwsEventType type,
                void *arg,
                uint8_t *data,
                size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client %u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client %u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    String msg = "";
    for (size_t i = 0; i < len; i++) msg += (char)data[i];

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      Serial.println("Invalid JSON");
      return;
    }

    String eventName = doc["event"].as<String>();
    JsonVariant eventData = doc["data"];
    wsEvents.emit(eventName, eventData, client);
  }
}

void initWebSocket(AsyncWebServer &server) {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serial.println("WebSocket Initialized");
  setupWebSocketHandlers();
}


// ==================== hanlers =================

static void setupWebSocketHandlers() {
  wsEvents.on("ping", [](JsonVariant data, AsyncWebSocketClient *client) {
    Serial.println("Received ping event");
    if (client) {
      client->text("pong");
    }
  });

  wsEvents.on("get-room", [](JsonVariant data, AsyncWebSocketClient *client) {
    int roomId = data["room"];
    Serial.printf("Client %u requested room %d\n", client->id(), roomId);

    if (!LittleFS.exists("/database.json")) {
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docDBRooms;

    if (!loadJsonFromFile("/database.json", docDBRooms)) {
      client->text(makeJsonMessageWS("error", "Gagal membaca database"));
      return;
    }

    JsonArray arrRooms = docDBRooms["rooms"].as<JsonArray>();
    JsonObject foundRoom;
    for (JsonObject room : arrRooms) {
      if (room["id"] == roomId) {
        foundRoom = room;
        break;
      }
    }

    if (foundRoom.isNull()) {
      client->text(makeJsonMessageWS("error", "Ruangan tidak ditemukan"));
      return;
    }

    client->text(makeJsonDataWS("room-data", foundRoom) );
  });

  wsEvents.on("get-settings", [](JsonVariant data, AsyncWebSocketClient *client) {
    Serial.printf("Client %u requested settings\n", client->id());

    if (!LittleFS.exists("/database.json")) {
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docSettings;

    if (!loadJsonFromFile("/database.json", docSettings)) {
      client->text(makeJsonMessageWS("error", "Gagal membaca settings"));
      return;
    }

    JsonObject settingObj = docSettings["settings"].as<JsonObject>();

    client->text(makeJsonDataWS("settings-data", settingObj) );
  });
}