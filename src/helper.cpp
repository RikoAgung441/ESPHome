#include "helper.h"


String readJsonFile(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        return "";
    }

    String content = file.readString();
    file.close();
    return content;
}

bool loadJsonFromFile(const char *path, JsonDocument &doc) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("Gagal membuka file: %s\n", path);
        return false;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.printf("Gagal parse JSON (%s)\n", error.c_str());
        return false;
    }

    return true;
}