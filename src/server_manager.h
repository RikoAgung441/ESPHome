#ifndef SERVER_MANAGER
#define SERVER_MANAGER

#include <WebServer.h>
#include <Arduino.h>


extern WebServer server;

void webServerInit();
void webServerHandleClient();
void sendJSON(int code, const String &json);

#endif //SERVER_MANAGER
