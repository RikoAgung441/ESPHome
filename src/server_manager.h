#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include <ESPAsyncWebServer.h>
#include <Arduino.h>


extern AsyncWebServer server;

void webServerInit();
static void handlerReqBody();
void sendJSON(AsyncWebServerRequest *request, int code, const String &json );
// void handleSave(AsyncWebServerRequest *request);

#endif //SERVER_MANAGER
