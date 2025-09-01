#include <WiFi.h>
#include "preferences_manager.h"
#include "server_manager.h"
#include "connection_config.h"
#include <DNSServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;

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

void handleSave(){
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("oldpass")) {
    String newSSID = server.arg("ssid");
    String newPass = server.arg("password");
    String oldPass = server.arg("oldpass");

    // Validasi password lama
    if (oldPass == ap_password) {
      // Simpan ke NVS
      preferences.begin("ap-config", false);
      preferences.putString("ssid", newSSID);
      preferences.putString("password", newPass);
      preferences.end();

      // Update variabel global
      ap_ssid = newSSID;
      ap_password = newPass;

      // Restart AP dengan SSID & Password baru TANPA reboot
      // WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
      // WiFi.softAPdisconnect(true);
      // delay(500);

      //set flag untuk restart
      // pendingRestart = true;
      // restartAt = millis() + 3000;

      // String msg = "<h1>Data berhasil disimpan!</h1>"
      //              "<p>Silakan hubungkan ke SSID baru: <b>" + ap_ssid + "</b></p>";
      // server.send(200, "text/html", msg);
    } else {
      // String msg = "<h1>Password salah!</h1>";
      // server.send(200, "text/html", msg);
    }
  }
}

String wifiManagerGetIP() {
  if (WiFi.getMode() & WIFI_AP) {
    return WiFi.softAPIP().toString();
  }
  return WiFi.localIP().toString();
}