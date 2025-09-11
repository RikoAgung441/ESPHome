#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"
// #include "spiff_manager.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "preferences_manager.h"

AsyncWebServer server(80);

// void listSPIFFSFiles() {
//   Serial.println("Daftar file di SPIFFS:");
//   File root = SPIFFS.open("/");
//   File file = root.openNextFile();
//   while (file) {
//     Serial.print("  ");
//     Serial.print(file.name());
//     Serial.print(" (");
//     Serial.print(file.size());
//     Serial.println(" bytes)");
//     file = root.openNextFile();
//   }
// }

void webServerInit() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS gagal mount!");
  }

  // handlerReqBody();
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  
  server.on("/wifi_configuration", HTTP_GET, [](AsyncWebServerRequest *request){
    if (SPIFFS.exists("/wifi_config.html")) {
      request->send(SPIFFS, "/wifi_config.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: /wifi_config.html tidak ditemukan di SPIFFS!");
    }
  });
  // server.on("/save", HTTP_POST, handleSave);

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, World!");
  });

  server.on("/api/test", HTTP_POST, [](AsyncWebServerRequest *request){
    preferences.begin("test", true); // read-only
    String ssid = preferences.getString("ssid", "");
    Serial.println(ssid);
    preferences.end();

    JsonDocument doc;
    doc["success"] = true;
    doc["ssid"] = ssid;

    String json;
    serializeJson(doc, json);
    // request->send(200, "application/json", "{\"success\":true}");
    request->send(200, "application/json", json);
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not found");
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/save", [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject obj = json.as<JsonObject>();
    String key = obj["key"];
    String value = obj["value"];
    Serial.printf("KEY: %s, VALUE: %s\n", key.c_str(), value.c_str());
    preferences.begin("test", false);
    preferences.putString(key.c_str(), value);
    preferences.end();
    request->send(200, "application/json", "{\"msg\":\"Data diterima\"}");
  });

  server.addHandler(handler);

  server.begin();
  Serial.println("Web server started!");
  
}

void sendJSON(AsyncWebServerRequest *request, int code, const String &json) {
  request->send(code, "application/json", json);
}



// void handleSave(AsyncWebServerRequest *request) {
//   Serial.println("🔄 Menyimpan config...");
//   if (request->method() != HTTP_POST) {
//     sendJSON(request, 405, "{\"error\":\"Method Not Allowed\"}");
//     return;
//   }
//   if (!request->hasArg("plain")) {
//     sendJSON(request,400, "{\"error\":\"No body\"}");
//     return;
//   }

//   String body = request->arg("plain");
//   int ssidPos = body.indexOf("\"ssid\"");
//   int passPos = body.indexOf("\"pass\"");
//   if (ssidPos < 0 || passPos < 0) {
//     sendJSON(request, 400, "{\"error\":\"Invalid JSON\"}");
//     return;
//   }

//   auto extract = [&](const String& key)->String {
//     int k = body.indexOf("\"" + key + "\"");
//     int colon = body.indexOf(":", k);
//     int firstQuote = body.indexOf("\"", colon + 1);
//     int secondQuote = body.indexOf("\"", firstQuote + 1);
//     if (k < 0 || colon < 0 || firstQuote < 0 || secondQuote < 0) return String("");
//     return body.substring(firstQuote + 1, secondQuote);
//   };

//   String ssid = extract("ssid");
//   String pass = extract("pass");

//   Serial.println("SSID: " + ssid);
//   Serial.println("PASS: " + pass);

//   sendJSON(request, 200, "{\"ok\":true,\"message\":\"WiFi disimpan, device akan restart...\"}");
//   delay(500);
// }


