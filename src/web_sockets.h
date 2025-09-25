#ifndef SRC_WEB_SOCKETS_H
#define SRC_WEB_SOCKETS_H
/* code */

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

extern AsyncWebSocket ws;

void initWebSocket(AsyncWebServer &server);

void broadcast(const String &eventName, JsonVariant data);

typedef void (*EventHandler)(AsyncWebSocketClient *client, JsonVariant data);

void on(const String &eventName, EventHandler handler);

#endif //SRC_WEB_SOCKETS_H
