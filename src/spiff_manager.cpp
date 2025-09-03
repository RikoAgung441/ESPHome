#include "spiff_manager.h"
#include <SPIFFS.h>

void listSPIFFSFiles() {
  Serial.println("Daftar file di SPIFFS:");
  File root = SPIFFS.open("/");
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