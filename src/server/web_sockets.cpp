#include <server/web_sockets.h>
#include <map>
#include "pzem_config.h"
#include <LittleFS.h>
#include "helper.h"
#include "debug.h"

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
    LOG_INFO("WebSocket client %u connected", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    LOG_INFO("WebSocket client %u disconnected", client->id());
  } else if (type == WS_EVT_DATA) {
    String msg = "";
    for (size_t i = 0; i < len; i++) msg += (char)data[i];

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      LOG_ERROR("Invalid JSON");
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
  
  setupWebSocketHandlers();
}


// ==================== hanlers =================

static void setupWebSocketHandlers() {
  wsEvents.on("ping", [](JsonVariant data, AsyncWebSocketClient *client) {
    LOG_INFO("Received ping event");
    if (client) {
      client->text("pong");
    }
  });
}