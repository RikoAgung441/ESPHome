#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>


extern AsyncWebServer server;

void webServerInit();
static void endpointSetting ();
static void endpointRooms ();
