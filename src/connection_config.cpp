#include <WiFi.h>
#include <server/server_manager.h>
#include "connection_config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "debug.h"
#include "helper.h"

const byte DNS_PORT = 53;
DNSServer dnsServer;

String ap_ssid ;
String ap_password ;
JsonDocument scanDoc;
bool scanRunning = false;
bool scanReady = false;


void connectionInit() {
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.begin("Fuz_pc", "MahfuZ56");

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Serial.println("\nWiFi terhubung!");
  // Serial.print("IP Address ESP32: ");
  // Serial.println(WiFi.localIP());

  // ketika di database tidak ada ssid dan password mode wifi pakai AP ambil dari database.json

  ap_ssid = "ESP32";
  ap_password = "12345678";
  IPAddress apIP(192, 168, 4, 10);
  IPAddress apGateway(192, 168, 4, 1);
  IPAddress apSubnet(255, 255, 255, 0);
  IPAddress ip, gateway, subnet, dns1, dns2;

  WiFi.softAPConfig(apIP, apGateway, apSubnet);


  if (!LittleFS.exists("/database.json")) {
    LOG_ERROR("File database.json tidak ditemukan");
    return;
  }

  JsonDocument docSettings;

  if (!loadJsonFromFile("/database.json", docSettings)) {
    LOG_ERROR("Gagal membaca database.json");
    return;
  }

  JsonObject settingObj = docSettings["settings"].as<JsonObject>();
  String ssidAp = settingObj["ssidAp"].as<String>();
  String passwordAp = settingObj["passwordAp"].as<String>();
  String ssid = settingObj["ssid"].as<String>();
  String password = settingObj["password"].as<String>();
  const char* static_ip_str = settingObj["static_ip"];
  const char* gateway_str = settingObj["gateway"];
  const char* subnet_str = settingObj["subnet"];
  const char* dns1_str = settingObj["dns1"];
  const char* dns2_str = settingObj["dns2"];

  
if (!ip.fromString(static_ip_str)) {
  Serial.println("IP address tidak valid");
}

if (!gateway.fromString(gateway_str)) {
  Serial.println("Gateway tidak valid");
}

if (!subnet.fromString(subnet_str)) {
  Serial.println("Subnet tidak valid");
}

if (!dns1.fromString(dns1_str)) {
  Serial.println("DNS1 tidak valid");
}

if (!dns2.fromString(dns2_str)) {
  Serial.println("DNS2 tidak valid");
}


  // if (!settingObj["static_ip"].as<String>().isEmpty()) {
  //   WiFi.config(static_ip, gateway, subnet, dns1, dns2);
  // }

  if(ssid.isEmpty() || password.isEmpty()) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  } else if ( ssidAp.isEmpty() || passwordAp.isEmpty() && !ssid.isEmpty() && !password.isEmpty() ) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("\nWiFi terhubung!");
    Serial.print("IP Address ESP32: ");
    Serial.println(WiFi.localIP());
  } else{
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    WiFi.softAP(ssidAp.c_str(), passwordAp.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi terhubung!");
    Serial.print("IP Address ESP32: ");
    Serial.println(WiFi.localIP());

    Serial.print("AP IP Address ESP32: ");
    Serial.println(WiFi.softAPIP());
  }

  LOG_INFO("Starting AP with SSID: %s", ap_ssid.c_str());


  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}



void startWifiScan() {
  if (scanRunning) return;

  scanDoc.clear();
  WiFi.scanNetworks(true); // async
  scanRunning = true;
  scanReady = false;
}

bool isWifiScanRunning() {
  return scanRunning;
}

bool isWifiScanReady() {
  return scanReady;
}

JsonArray getWifiScanResult() {
  return scanDoc.as<JsonArray>();
}

void resetWifiScan() {
  scanRunning = false;
  scanReady = false;
  scanDoc.clear();
}

void handleWifiScanLoop() {
  if (!scanRunning) return;

  int n = WiFi.scanComplete();
  if (n < 0) return;

  JsonArray arr = scanDoc.to<JsonArray>();
  for (int i = 0; i < n; i++) {
    JsonObject net = arr.add<JsonObject>();
    net["ssid"] = WiFi.SSID(i);
    net["strength"] = WiFi.RSSI(i);
    net["encryption"] =
      (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "secured";
  }

  WiFi.scanDelete();
  scanRunning = false;
  scanReady = true;
}
