#ifndef HELPER_H
#define HELPER_H
/* code */

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>


String readJsonFile(const char *path);
bool loadJsonFromFile(const char *path, JsonDocument &doc);
// String responseJson()

#endif //HELPER_H
