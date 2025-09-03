#ifndef CONNECTION_CONFIG
#define CONNECTION_CONFIG

#include <Arduino.h>
#include <DNSServer.h>


void connectionInit();
void startAPMode();
void resetToDefault();
// void handleSave();
String wifiManagerGetIP();

extern String ap_ssid, ap_password;
extern DNSServer dnsServer;

#endif //CONNECTION_CONFIG
