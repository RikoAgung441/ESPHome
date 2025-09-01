#ifndef CONNECTION_CONFIG
#define CONNECTION_CONFIG

#include <Arduino.h>

void connectionInit();
void startAPMode();
void resetToDefault();
void handleSave();
String wifiManagerGetIP();
String ap_ssid, ap_password;

extern DNSServer dnsServer;

#endif //CONNECTION_CONFIG
