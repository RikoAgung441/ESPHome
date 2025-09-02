#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"

WebServer server(80);

void listSPIFFSFiles() {
  Serial.println("Daftar file di SPIFFS:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" (");
    Serial.print(file.size());
    Serial.println(" bytes)");
    file = root.openNextFile();
  }
}

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
  } else {
    listSPIFFSFiles(); // Panggil hanya jika mount berhasil
  }

  server.on("/", HTTP_GET, handleRoot);


  server.on(" /wifi_configuration", HTTP_GET, []() {
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


