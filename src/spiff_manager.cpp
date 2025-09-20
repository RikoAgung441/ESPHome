#include "spiff_manager.h"
#include <LittleFS.h>

void listSPIFFSFiles() {
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