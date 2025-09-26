#include "pzem_config.h"
#include "web_sockets.h"
#include <ArduinoJson.h>


HardwareSerial pzemSerial(2);
PZEM004Tv30 pzem( pzemSerial, PZEM_RX, PZEM_TX);


void initPzem() {
  pzemSerial.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);

  if(!pzemSerial){
    Serial.println("PZEM Serial not initialized");
    return;
  }

  Serial.println("PZEM Initialized");

  
}

float readPzemVoltage() {
  float voltage = pzem.voltage();
  if(voltage < 0.0){
    Serial.println("Error reading voltage");
    return -1;
  }

  return isnan(voltage) || voltage == 0 ? 0 : voltage;
}

float readPzemCurrent() {
  float current = pzem.current();
  if(current < 0.0){
    Serial.println("Error reading current");
    return -1;
  }

  return isnan(current) || current == 0 ? 0 : current;
}

float readPzemPower() {
  float power = pzem.power();
  if(power < 0.0){
    Serial.println("Error reading power");
    return -1;
  }

  return isnan(power) || power == 0 ? 0 : power;
}

float readPzemEnergy() {
  float energy = pzem.energy();
  if(energy < 0.0){
    Serial.println("Error reading energy");
    return -1;
  }

  return isnan(energy) || energy == 0 ? 0 : energy;
}

float readPzemFrequency() {
  float frequency = pzem.frequency();
  if(frequency < 0.0){
    Serial.println("Error reading frequency");
    return -1;
  }

  return isnan(frequency) || frequency == 0 ? 0 : frequency;
}

float readPzemPF() {
  float pf = pzem.pf();
  if(pf < 0.0){
    Serial.println("Error reading pf");
    return -1;
  }

  return isnan(pf) || pf == 0 ? 0 : pf;
}

void broadcastPzemData() {
  JsonDocument doc;
  doc["voltage"] = readPzemVoltage();
  doc["current"] = readPzemCurrent();
  doc["power"] = readPzemPower();
  doc["energy"] = readPzemEnergy();
  doc["frequency"] = readPzemFrequency();
  doc["pf"] = readPzemPF();

  broadcast("pzem_update", doc);
}

