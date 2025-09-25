#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include <ESPAsyncWebServer.h>
#include <Arduino.h>


extern AsyncWebServer server;

void webServerInit();
static void endpointSetting ();
static void endpointRooms ();

#endif //SERVER_MANAGER
