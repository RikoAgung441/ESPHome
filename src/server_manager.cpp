#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"

WebServer server(80);

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");\
  server.streamFile(file, "text/html");
  file.close();
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void webServerInit() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS gagal mount!");
  }

  server.on("/", HTTP_GET, handleRoot);


  server.on("/wifi_configuration", HTTP_GET, []() {
  File file = SPIFFS.open("/pages/wifi_configuration.html", "r");
  server.streamFile(file, "text/html");
  file.close();
  });

  server.onNotFound(handleNotFound);

  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println("Web server started!");
  // server.begin();
  
}

void sendJSON(int code, const String &json) {
  server.send(code, "application/json", json);
}


void webServerHandleClient() {
  server.handleClient();
}
