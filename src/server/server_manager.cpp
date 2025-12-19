#include <server/server_manager.h>
#include <LittleFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <controllers/relay_control.h> 
#include "helper.h"
#include <server/web_sockets.h>
#include "debug.h"

AsyncWebServer server(80);

void webServerInit(){
  DefaultHeaders::Instance().addHeader(
    "Access-Control-Allow-Origin", "*"
  );
  DefaultHeaders::Instance().addHeader(
    "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"
  );
  DefaultHeaders::Instance().addHeader(
    "Access-Control-Allow-Headers", "Content-Type, Authorization"
  );
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");


  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, World!"); 
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.begin();
  LOG_INFO("Web server started!");
}

