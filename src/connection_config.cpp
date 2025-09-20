#include <WiFi.h>
#include "server_manager.h"
#include "connection_config.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;

String ap_ssid ;
String ap_password ;

void connectionInit() {
  bool success = false;
  int retryCount = 0;

  while (retryCount < 3 && !success) {
    File file = SPIFFS.open("/database.json", "r");
    if (file) {
      JsonDocument doc; 
      DeserializationError error = deserializeJson(doc, file);
      if (!error) {
        ap_ssid = doc["setting"]["ssid"].as<String>();
        ap_password = doc["setting"]["password"].as<String>();
        success = true;
      } else {
        Serial.print("❌ Gagal parse JSON: ");
        Serial.println(error.c_str());
      }
      file.close();
    } else {
      Serial.println("❌ Gagal buka file ");
    }

    if (!success) {
      retryCount++;
      Serial.printf("🔁 Retry %d/3...\n", retryCount);
      delay(500);
    }
  }

  if (!success) {
    ap_ssid = "ESP32_Default";
    ap_password = "12345678";
    Serial.println("⚠️ Menggunakan SSID & Password default");
  }

  if (!WiFi.softAP(ap_ssid.c_str(), ap_password.c_str())) {
    Serial.println("❌ Gagal start AP, retry sekali lagi...");
    delay(1000);
    WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  }

  Serial.print("✅ AP Aktif: ");
  Serial.println(ap_ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}


void startAPMode(){
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  Serial.println("AP Mode Aktif");
  Serial.println(WiFi.softAPIP());
  ESP.restart();
}

void resetToDefault(){
  // preferences.begin("ap-config", false);
  // preferences.clear();
  // preferences.end();

  ap_ssid = "ESP32_Default";
  ap_password = "12345678";

  Serial.println("WiFi config direset ke default!");
  // blinkLED(2, 300); 

  WiFi.softAPdisconnect(true);
  delay(500);
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
}


String wifiManagerGetIP() {
  if (WiFi.getMode() & WIFI_AP) {
    return WiFi.softAPIP().toString();
  }
  return WiFi.localIP().toString();
}