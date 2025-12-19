#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>


String readJsonFile(const char *path);
bool loadJsonFromFile(const char *path, JsonDocument &doc);
String makeJsonMessageWS(const char* event, const char* msg);
String makeJsonDataWS(const char* event, JsonVariant data);
String makeJsonMessage(const char* msg);
String getString(JsonObject obj, const char* key);
bool getBool(JsonObject obj, const char* key, bool def = true);
String generateRandomToken(size_t length);
String generateId();

