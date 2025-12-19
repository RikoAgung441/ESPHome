#include "spiff_manager.h"
#include <LittleFS.h>
#include "debug.h"



void initLittleFS() {
  if (!LittleFS.begin(true)) {
    LOG_INFO("LittleFS gagal mount!");
  } else {
    LOG_INFO("LittleFS mounted successfully.");
    listLittleFiles();
  }
}

void listLittleFiles() {
  Serial.println("Daftar file di LittleFS:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    LOG_INFO("File: %s (size: %d bytes)", file.name(), file.size());
    file = root.openNextFile();
  }
}

void createDatabase() {
  if (!LittleFS.exists("/database.json")) {
    LOG_WARN("File database.json tidak ditemukan, membuat file baru.");
    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      LOG_ERROR("Gagal membuat file database.json");
      return;
    }
    file.print("{\"rooms\":[]}, \"settings\":{}}");
    file.close();
    LOG_INFO("File database.json berhasil dibuat.");
  } else {
    LOG_INFO("File database.json ditemukan.");
  }
}