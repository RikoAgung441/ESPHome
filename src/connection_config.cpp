#include <WiFi.h>
#include <server/server_manager.h>
#include "connection_config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "debug.h"

const byte DNS_PORT = 53;
DNSServer dnsServer;

String ap_ssid ;
String ap_password ;
JsonDocument doc;

void connectionInit() {
  bool success = false;
  int retryCount = 0;

  while (retryCount < 3 && !success) {
    File file = LittleFS.open("/database.json", "r");
    
    if (!file) {
      Serial.println("❌ Gagal membuka file database.json");
      retryCount++;
      delay(1000);
      continue;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    ap_ssid = doc["settings"]["ap_ssid"].as<String>();
    ap_password = doc["settings"]["ap_wifi_password"].as<String>();

    Serial.print("DB SSID: ");
    Serial.println(ap_ssid);
    Serial.print("DB Password: ");
    Serial.println(ap_password);

    if (ap_ssid.isEmpty() || ap_ssid == "null" || ap_password.isEmpty() || ap_password == "null") {
      Serial.println("❌ SSID atau Password kosong, menggunakan default");
      success = false;
      break;
    }

    success = true;
  }

  if (!success) {
    ap_ssid = "ESP32_Defaulty";
    ap_password = "11223344";
    Serial.println("⚠️ Menggunakan SSID & Password default");
  }

  if (!WiFi.softAP(ap_ssid.c_str(), ap_password.c_str())) {
    Serial.println("❌ Gagal start AP, retry sekali lagi...");
    Serial.print("SSID: ");
    Serial.println(ap_ssid);
    Serial.print("Password: ");
    Serial.println(ap_password);
    delay(2000);
    WiFi.softAP("ESP32_Defaulty", "11223344");
  }

  LOG_INFO("✅ AP Aktif: ");
  LOG_INFO("SSID: %s", ap_ssid.c_str());
  LOG_INFO("IP: %s", WiFi.softAPIP().toString().c_str());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // run scanNetworks and print value return
  JsonArray arr = scanNetworks();
  serializeJsonPretty(arr, Serial);
  Serial.println();
}



JsonArray scanNetworks() {
  doc.clear();

  LOG_INFO("🔍 Mencari jaringan WiFi...");
  int n = WiFi.scanNetworks();

  JsonArray arr = doc.to<JsonArray>();

  if (n == 0) {
    LOG_WARN("Tidak ada jaringan WiFi ditemukan");
    return arr; // array kosong
  }

  LOG_INFO("Ditemukan %d jaringan WiFi:", n);

  for (int i = 0; i < n; ++i) {
    JsonObject netObj = arr.add<JsonObject>();
    netObj["ssid"] = WiFi.SSID(i);
    netObj["rssi"] = WiFi.RSSI(i);
    netObj["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "secured";
  }

  return arr;
}
