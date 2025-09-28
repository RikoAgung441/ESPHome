#pragma once
#include <Arduino.h>
#include <DNSServer.h>


void connectionInit();
void startAPMode();
void resetToDefault();
// void handleSave();
String wifiManagerGetIP();

extern String ap_ssid, ap_password;
extern DNSServer dnsServer;

