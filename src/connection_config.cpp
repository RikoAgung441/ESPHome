#include <WiFi.h>
#include "server_manager.h"
#include "connection_config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;

String ap_ssid ;
String ap_password ;

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

  Serial.print("✅ AP Aktif: ");
  Serial.println(ap_ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // scanNetworks();
}


void scanNetworks() {
  Serial.println("🔍 Mencari jaringan WiFi...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("❌ Tidak ada jaringan WiFi ditemukan");
  } else {
    Serial.print("✅ Ditemukan ");
    Serial.print(n);
    Serial.println(" jaringan WiFi:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (RSSI: ");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) ");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      Serial.print(" -- BSSID: ");
      Serial.print(WiFi.encryptionType(i));
      Serial.println();
    }
  }
  Serial.println("");
}
