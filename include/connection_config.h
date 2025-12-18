#pragma once
#include <Arduino.h>
#include <DNSServer.h>

// enum class WifiScanState {
//   IDLE,
//   SCANNING,
//   READY
// };

// WifiScanState scanState = WifiScanState::IDLE;


void connectionInit();
void startWifiScan();
bool isWifiScanRunning();
bool isWifiScanReady();
JsonArray getWifiScanResult();
void handleWifiScanLoop();
void resetWifiScan();

extern String ap_ssid, ap_password;
extern DNSServer dnsServer;

