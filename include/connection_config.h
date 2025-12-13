#pragma once
#include <Arduino.h>
#include <DNSServer.h>


void connectionInit();
JsonArray scanNetworks();


extern String ap_ssid, ap_password;
extern DNSServer dnsServer;

