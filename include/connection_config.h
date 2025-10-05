#pragma once
#include <Arduino.h>
#include <DNSServer.h>


void connectionInit();
void scanNetworks();


extern String ap_ssid, ap_password;
extern DNSServer dnsServer;

