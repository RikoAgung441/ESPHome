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
    doc["payload"] = data;

    String output;
    serializeJson(doc, output);
    return output;
}

String makeJsonMessage(const char* msg) {
    JsonDocument doc;
    doc["message"] = msg;

    String output;
    serializeJson(doc, output);
    return output;
}

String getString(JsonObject obj, const char* key) {
    return obj[key].is<const char*>() ? String(obj[key].as<const char*>()) : "";
}

bool getBool(JsonObject obj, const char* key, bool def) {
    return obj[key].is<bool>() ? obj[key].as<bool>() : def;
}

String generateRandomToken(size_t length) {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    String token;
    token.reserve(length + 10);
    for (size_t i = 0; i < length; i++) {
        token += charset[random(0, max_index)];
    }

    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
       return "";
    }

    JsonDocument docDatabase;
    if (!loadJsonFromFile("/database.json", docDatabase)) {
      LOG_ERROR("Gagal membaca database.json");
      return "";
    }

    JsonObject DBConfig = docDatabase["config"].as<JsonObject>();
    DBConfig["token"] = token;

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      LOG_ERROR("Gagal membuka database.json untuk penulisan");
      return "";
    }
    serializeJson(docDatabase, file);
    file.close();
    
    return token;
}

String generateId() {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    String id;
    id.reserve(12);
    for (size_t i = 0; i < 8; i++) {
        id += charset[random(0, max_index)];
    }
    return id;
}


