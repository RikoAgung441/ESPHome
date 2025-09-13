#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include <ESPAsyncWebServer.h>
#include <Arduino.h>


extern AsyncWebServer server;

void webServerInit();
void sendJSON(AsyncWebServerRequest *request, int code, const String &json );
static void endpointSetting ();
static void endpointRooms ();

#endif //SERVER_MANAGER
