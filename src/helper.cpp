#include "helper.h"
#include "debug.h"


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
        LOG_INFO("Gagal membuka file: %s\n", path);
        return false;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        LOG_INFO("Gagal parse JSON (%s)\n", error.c_str());
        return false;
    }

    return true;
}

String makeJsonMessageWS(const char* event, const char* msg) {
    JsonDocument doc;
    doc["event"] = event;
    doc["data"]["msg"] = msg;

    String output;
    serializeJson(doc, output);
    return output;
}

String makeJsonDataWS(const char* event, JsonVariant data) {
    JsonDocument doc;
    doc["event"] = event;
    doc["data"] = data;

    String output;
    serializeJson(doc, output);
    return output;
}

String makeJsonMessage(const char* msg) {
    JsonDocument doc;
    doc["msg"] = msg;

    String output;
    serializeJson(doc, output);
    return output;
}
