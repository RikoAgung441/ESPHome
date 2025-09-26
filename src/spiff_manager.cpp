#include "spiff_manager.h"
#include <LittleFS.h>



void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS gagal mount!");
    // createDatabase();
  } else {
    Serial.println("LittleFS mounted successfully.");
    listLittleFiles();
  }
}

void listLittleFiles() {
  Serial.println("Daftar file di LittleFS:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" (");
    Serial.print(file.size());
    Serial.println(" bytes)");
    file = root.openNextFile();
  }
}

void createDatabase() {
  if (!LittleFS.exists("/database.json")) {
    Serial.println("File database.json tidak ditemukan, membuat file baru.");
    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      Serial.println("Gagal membuat file database.json");
      return;
    }
    file.print("{\"rooms\":[]}, \"settings\":{}}");
    file.close();
    Serial.println("File database.json berhasil dibuat.");
  } else {
    Serial.println("File database.json ditemukan.");
  }
}