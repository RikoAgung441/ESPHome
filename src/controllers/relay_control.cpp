#include <controllers/relay_control.h>
#include "debug.h"


void relaySwitch(int pin, bool state) {
  LOG_INFO("Relay Switch ID: %d State: %d", pin, state);
  digitalWrite(pin, state ? HIGH : LOW);
  LOG_INFO("Relay %d %s", pin, state ? "ON" : "OFF");
}