#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"

WebServer server(80);

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
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

  server.on("/ip", HTTP_GET, []() {
    server.send(200, "text/plain", wifiManagerGetIP());
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Web server started!");
    // server.on("/save", HTTP_POST, handleSave);
  // server.begin();
  
}

void webServerHandleClient() {
  server.handleClient();
}
