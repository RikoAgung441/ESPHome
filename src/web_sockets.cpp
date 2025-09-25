#include "web_sockets.h"
#include <map>

AsyncWebSocket ws("/ws");

std::map<String, EventHandler> eventHandlers;

void on(const String &eventName, EventHandler handler) {
  eventHandlers[eventName] = handler;
}

void emit(const String &eventName, AsyncWebSocketClient *client, JsonVariant data) {
  if (eventHandlers.count(eventName)) {
    eventHandlers[eventName](client, data);
  }
}

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
    emit(eventName, client, eventData);
  }
}

void initWebSocket(AsyncWebServer &server) {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
}
