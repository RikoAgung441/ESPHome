#pragma once
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <map>
#include <functional>

extern AsyncWebSocket ws;

class EventEmitter {
  using Handler = std::function<void(JsonVariant, AsyncWebSocketClient*)>;
  std::map<String, Handler> listeners;

public:
  void on(const String &eventName, Handler handler) {
    listeners[eventName] = handler;
  }

  void emit(const String &eventName, JsonVariant data, AsyncWebSocketClient *client) {
    if (listeners.count(eventName)) {
      listeners[eventName](data, client);
    }
  }
};

extern EventEmitter wsEvents;

void initWebSocket(AsyncWebServer &server);
void broadcast(const String &eventName, JsonVariant data);
static void setupWebSocketHandlers();

