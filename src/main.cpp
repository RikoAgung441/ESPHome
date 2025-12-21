#include <Arduino.h>
#include <WiFi.h>
#include <server/server_manager.h>
#include <connection_config.h>
#include <LittleFS.h>
#include <server/web_sockets.h>
#include <spiff_manager.h>
#include <pins_config.h>
#include <pzem_config.h>
#include <handler/handler.h>

volatile bool buttonInterrupt = false;
unsigned long buttonPressTime = 0;
bool buttonHeld = false;
bool pendingRestart = false;
unsigned long restartAt = 0;
unsigned long lastMillis = 0;

// ====== INTERRUPT ======
void IRAM_ATTR handleButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {  
    buttonPressTime = millis();
    buttonHeld = true;
  } else {
    buttonInterrupt = true;  // Lepas tombol
    buttonHeld = false;
  }
}

// ====== LED Blink Utility ======
void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < 8; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], LOW); // Matikan semua relay saat start
  }

  // LittleFS.format();


  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, CHANGE);

  initLittleFS();
  connectionInit();
  webServerInit();
  initWebSocket(server);
  initHandlers();
}


// ====== Loop ======
void loop() {
  dnsServer.processNextRequest();

  handleWifiScanLoop(); 

  // if (millis() - lastMillis > 2000) {
  //   broadcastPzemData();

  //   lastMillis = millis();
  // }



  // reset ke default jika tombol ditekan lebih dari 5 detik

  // if (buttonHeld && (millis() - buttonPressTime > 5000)) {
  //   buttonHeld = false;
  //   resetToDefault();
  // }

  // -----------------------------

//     if (pendingRestart && millis() > restartAt) {
//     pendingRestart = false;
//     restartAt = 0;
//     Serial.println("🔄 Restarting AP dengan SSID baru...");

//     WiFi.softAPdisconnect(true);
//     delay(300);
//     WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());

//     Serial.print("✅ AP Baru Aktif: "); Serial.println(ap_ssid);
//     Serial.print("IP: "); Serial.println(WiFi.softAPIP());
//     startAPMode();
//   }
// }

// void relayControl(int relayIndex, bool state) {
//   if (relayIndex < 1 || relayIndex > 4) return; // Validasi index
//     digitalWrite(relayPin[relayIndex-1], state ? HIGH : LOW);
//     Serial.printf("Relay %d %s\n", relayIndex, state ? "ON" : "OFF");
}