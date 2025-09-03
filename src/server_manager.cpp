#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"

AsyncWebServer server(80);

void webServerInit() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS gagal mount!");
  } else {
    listSPIFFSFiles(); // Panggil hanya jika mount berhasil
  }

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not found");
  });


  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println("Web server started!");
  
}

void sendJSON(AsyncWebServerRequest *request, int code, const String &json) {
  request->send(code, "application/json", json);
}


void handleSave(AsyncWebServerRequest *request) {
  Serial.println("🔄 Menyimpan config...");
  if (request->method() != HTTP_POST) {
    sendJSON(request, 405, "{\"error\":\"Method Not Allowed\"}");
    return;
  }
  if (!request->hasArg("plain")) {
    sendJSON(request,400, "{\"error\":\"No body\"}");
    return;
  }

  String body = request->arg("plain");
  int ssidPos = body.indexOf("\"ssid\"");
  int passPos = body.indexOf("\"pass\"");
  if (ssidPos < 0 || passPos < 0) {
    sendJSON(request, 400, "{\"error\":\"Invalid JSON\"}");
    return;
  }

  auto extract = [&](const String& key)->String {
    int k = body.indexOf("\"" + key + "\"");
    int colon = body.indexOf(":", k);
    int firstQuote = body.indexOf("\"", colon + 1);
    int secondQuote = body.indexOf("\"", firstQuote + 1);
    if (k < 0 || colon < 0 || firstQuote < 0 || secondQuote < 0) return String("");
    return body.substring(firstQuote + 1, secondQuote);
  };

  String ssid = extract("ssid");
  String pass = extract("pass");

  Serial.println("SSID: " + ssid);
  Serial.println("PASS: " + pass);

  sendJSON(request, 200, "{\"ok\":true,\"message\":\"WiFi disimpan, device akan restart...\"}");
  delay(500);
}


