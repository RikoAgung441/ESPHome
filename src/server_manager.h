#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include <WebServer.h>
#include <Arduino.h>


extern WebServer server;

void webServerInit();
void webServerHandleClient();

#endif //SERVER_MANAGER
