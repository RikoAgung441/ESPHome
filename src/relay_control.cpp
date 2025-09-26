#include "relay_control.h"


void relaySwitch(int pin, bool state) {
  Serial.print("Relay Switch ID: "); Serial.print(pin); Serial.print(" State: "); Serial.println(state);
  digitalWrite(pin, state ? HIGH : LOW);
  Serial.print("Relay "); Serial.print(pin); Serial.print(" "); Serial.println(state ? "ON" : "OFF");

}