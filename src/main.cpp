#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

#define BUTTON_PIN 12   // Tombol BOOT (GPIO)
#define LED_PIN 2      // LED indikator

volatile bool buttonInterrupt = false;
unsigned long buttonPressTime = 0;
bool buttonHeld = false;
bool pendingRestart = false;
unsigned long restartAt = 0;

String ap_ssid, ap_password;
bool apModeActive = false;

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

// ====== Web Handler ======
void handleRoot() {
  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>Konfigurasi WiFi ESP32</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        background: #f4f4f4;
        text-align: center;
        padding: 20px;
      }
      .card {
        background: #fff;
        max-width: 400px;
        margin: auto;
        padding: 20px;
        border-radius: 12px;
        box-shadow: 0 4px 8px rgba(0,0,0,0.2);
      }
      h1 {
        font-size: 22px;
        margin-bottom: 20px;
      }
      input[type=text], input[type=password] {
        width: 90%;
        padding: 10px;
        margin: 10px 0;
        border: 1px solid #ccc;
        border-radius: 6px;
      }
      input[type=submit] {
        background: #007BFF;
        color: white;
        border: none;
        padding: 12px 20px;
        border-radius: 6px;
        cursor: pointer;
        font-size: 16px;
      }
      input[type=submit]:hover {
        background: #0056b3;
      }
      .footer {
        margin-top: 15px;
        font-size: 12px;
        color: #555;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Konfigurasi WiFi ESP32</h1>
      <form action="/save" method="POST">
        <input type="text" name="ssid" placeholder="SSID Baru"><br>
        <input type="password" name="oldpass" placeholder="Password Lama"><br>
        <input type="password" name="password" placeholder="Password Baru"><br>
        <input type="submit" value="Simpan">
      </form>
      <div class="footer">ESP32 Config Portal</div>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("oldpass")) {
    String newSSID = server.arg("ssid");
    String newPass = server.arg("password");
    String oldPass = server.arg("oldpass");

    // Validasi password lama
    if (oldPass == ap_password) {
      // Simpan ke NVS
      preferences.begin("ap-config", false);
      preferences.putString("ssid", newSSID);
      preferences.putString("password", newPass);
      preferences.end();

      // Update variabel global
      ap_ssid = newSSID;
      ap_password = newPass;

      // Restart AP dengan SSID & Password baru TANPA reboot
      // WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
      // WiFi.softAPdisconnect(true);
      // delay(500);

      //set flag untuk restart
      pendingRestart = true;
      restartAt = millis() + 3000;

      String msg = "<h1>Data berhasil disimpan!</h1>"
                   "<p>Silakan hubungkan ke SSID baru: <b>" + ap_ssid + "</b></p>";
      server.send(200, "text/html", msg);

      Serial.println("SSID/Password baru disimpan. AP di-restart.");

    } else {
      server.send(403, "text/html", "<h1>Password lama salah! Konfigurasi ditolak.</h1>");
    }
  } else {
    server.send(400, "text/html", "Form tidak lengkap!");
  }
}

// ====== AP Mode ======
void startAPMode() {
  apModeActive = true;
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  Serial.println("AP Mode Aktif");
  Serial.println(WiFi.softAPIP());

  blinkLED(2, 300); // kedip lambat 2x saat masuk AP mode

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}

// ====== Reset Default ======
void resetToDefault() {
  preferences.begin("ap-config", false);
  preferences.clear();
  preferences.end();

  ap_ssid = "ESP32_Default";
  ap_password = "12345678";

  Serial.println("WiFi config direset ke default!");
  blinkLED(3, 150); // kedip cepat 3x sebagai tanda reset sukses

  WiFi.softAPdisconnect(true);
  delay(500);
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, CHANGE);

  // Ambil SSID & Password terakhir
  preferences.begin("ap-config", true);
  ap_ssid = preferences.getString("ssid", "");
  ap_password = preferences.getString("password", "");
  preferences.end();

  // Jika kosong atau invalid -> fallback ke default
  if (ap_ssid == "" || ap_password.length() < 8) {
    ap_ssid = "ESP32_Default";
    ap_password = "12345678";
    Serial.println("⚠️ Config kosong, pakai default");
  }

  // Mulai AP Mode
  if (!WiFi.softAP(ap_ssid.c_str(), ap_password.c_str())) {
    Serial.println("❌ Gagal start AP, fallback ke default");
    WiFi.softAP("ESP32_Default", "12345678");
  }
  startAPMode();
  Serial.print("✅ AP Aktif: ");
  Serial.println(ap_ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}


// ====== Loop ======
void loop() {
  // Jalankan web server
  if (apModeActive) {
    server.handleClient();
  }

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
 