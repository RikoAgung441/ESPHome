#include <Arduino.h>
#include <WiFi.h>
#include <server_manager.h>
#include <connection_config.h>


#define BUTTON_PIN 12   // Tombol BOOT (GPIO)
#define LED_PIN 2      // LED indikator
int relayPin[4] = {14, 27, 26, 25}; // Relay control pins

volatile bool buttonInterrupt = false;
unsigned long buttonPressTime = 0;
bool buttonHeld = false;
bool pendingRestart = false;
unsigned long restartAt = 0;

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

  for (int i = 0; i < 3; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], LOW); // Matikan semua relay saat start
  }

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, CHANGE);

  connectionInit();
  webServerInit();
}


// ====== Loop ======
void loop() {
  // Jalankan web server
  dnsServer.processNextRequest();

  

  // Reset ke default jika tombol > 5 detik
  if (buttonHeld && (millis() - buttonPressTime > 5000)) {
    buttonHeld = false;
    resetToDefault();
  }

    if (pendingRestart && millis() > restartAt) {
    pendingRestart = false;
    restartAt = 0;
    Serial.println("🔄 Restarting AP dengan SSID baru...");

    WiFi.softAPdisconnect(true);
    delay(300);
    WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());

    Serial.print("✅ AP Baru Aktif: "); Serial.println(ap_ssid);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
    startAPMode();
  }
}

void relayControl(int relayIndex, bool state) {
  if (relayIndex < 1 || relayIndex > 4) return; // Validasi index
    digitalWrite(relayPin[relayIndex-1], state ? HIGH : LOW);
    Serial.printf("Relay %d %s\n", relayIndex, state ? "ON" : "OFF");
}