#include <server_manager.h>
#include <SPIFFS.h>
#include <FS.h>
#include "connection_config.h"
#include "spiff_manager.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

void webServerInit() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS gagal mount!");
  } else {
    listSPIFFSFiles();
  }

  // server.on("/save", HTTP_POST, handleSave);
  handlerReqBody();
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/wifi_configuration", HTTP_GET, [](AsyncWebServerRequest *request){
    if (SPIFFS.exists("/wifi_config.html")) {
      request->send(SPIFFS, "/wifi_config.html", "text/html");
    } else {
      request->send(404, "text/plain", "Error: /wifi_config.html tidak ditemukan di SPIFFS!");
    }
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not found");
  });

  server.begin();
  Serial.println("Web server started!");
  
}

void sendJSON(AsyncWebServerRequest *request, int code, const String &json) {
  request->send(code, "application/json", json);
}

static void handlerReqBody() {
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data,
                          size_t len, size_t index, size_t total) {
    String body;
    body.reserve(len);
    for (size_t i = 0; i < len; i++) {
      body += (char)data[i];
    }

    Serial.printf("\nRequest ke %s\n", request->url().c_str());
    Serial.println("Body:");
    Serial.println(body);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    if (request->url() == "/save") {
      const char* key = doc["key"];
      int value = doc["value"];
      Serial.printf("➡ Save Data | key: %s, value: %s\n", key, value);

      request->send(200, "application/json", "{\"msg\":\"Data saved\"}");
    } 
    // else if (request->url() == "/lamp") {
    //   bool state = doc["state"]; 
    //   Serial.printf("➡ Lamp Control | State: %s\n", state ? "ON" : "OFF");

    
    //   digitalWrite(2, state ? HIGH : LOW);

    //   request->send(200, "application/json", "{\"msg\":\"Lamp updated\"}");
    // }
  });
}

// void getSettings() {
//   server.on("/save", HTTP_POST, 
//   [](AsyncWebServerRequest *request) {
//       if (!request->hasArg("plain")) {
//           request->send(400, "application/json", "{\"error\":\"No body\"}");
//           return;
//       }

//       String body = request->arg("plain");
//       // lakukan parsing JSON di sini kalau perlu
//       request->send(200, "application/json", "{\"success\":true}");
//   }
// );

// }


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


