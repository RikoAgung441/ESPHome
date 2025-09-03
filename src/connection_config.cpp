#include <WiFi.h>
#include "preferences_manager.h"
#include "server_manager.h"
#include "connection_config.h"

const byte DNS_PORT = 53;
DNSServer dnsServer;

String ap_ssid = "ESP32_Default";
String ap_password = "12345678";

void connectionInit(){
  preferences.begin("ap-config", true);
  ap_ssid = preferences.getString("ssid", "");
  ap_password = preferences.getString("password", "");
  preferences.end();

  if (ap_ssid == "" || ap_password.length() < 8) {
    ap_ssid = "ESP32_Default";
    ap_password = "12345678";
    Serial.println("⚠️ Config kosong, pakai default");
  }

  if (!WiFi.softAP(ap_ssid.c_str(), ap_password.c_str())) {
    Serial.println("❌ Gagal start AP, fallback ke default");
    WiFi.softAP("ESP32_Default", "12345678");
  }
  // startAPMode();
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
  // ESP.restart();
}

void resetToDefault(){
  preferences.begin("ap-config", false);
  preferences.clear();
  preferences.end();

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