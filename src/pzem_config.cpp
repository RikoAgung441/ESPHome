#include "pzem_config.h"
#include <server/web_sockets.h>
#include <ArduinoJson.h>
#include "debug.h"


HardwareSerial pzemSerial(2);
PZEM004Tv30 pzem( pzemSerial, PZEM_RX, PZEM_TX);


void initPzem() {
  pzemSerial.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);

  if(!pzemSerial){
    LOG_INFO("PZEM Serial not initialized");
    return;
  }

  LOG_INFO("PZEM Initialized");

  
}

float readPzemVoltage() {
  float voltage = pzem.voltage();
  if(voltage < 0.0){
    LOG_ERROR("Error reading voltage");
    return -1;
  }

  return isnan(voltage) || voltage == 0 ? 0 : voltage;
}

float readPzemCurrent() {
  float current = pzem.current();
  if(current < 0.0){
    LOG_ERROR("Error reading current");
    return -1;
  }

  return isnan(current) || current == 0 ? 0 : current;
}

float readPzemPower() {
  float power = pzem.power();
  if(power < 0.0){
    LOG_ERROR("Error reading power");
    return -1;
  }

  return isnan(power) || power == 0 ? 0 : power;
}

float readPzemEnergy() {
  float energy = pzem.energy();
  if(energy < 0.0){
    LOG_ERROR("Error reading energy");
    return -1;
  }

  return isnan(energy) || energy == 0 ? 0 : energy;
}

float readPzemFrequency() {
  float frequency = pzem.frequency();
  if(frequency < 0.0){
    LOG_ERROR("Error reading frequency");
    return -1;
  }

  return isnan(frequency) || frequency == 0 ? 0 : frequency;
}

float readPzemPF() {
  float pf = pzem.pf();
  if(pf < 0.0){
    LOG_ERROR("Error reading pf");
    return -1;
  }

  return isnan(pf) || pf == 0 ? 0 : pf;
}

void broadcastPzemData() {
  JsonDocument doc;
  // doc["voltage"] = readPzemVoltage();
  // doc["current"] = readPzemCurrent();
  // doc["power"] = readPzemPower();
  // doc["energy"] = readPzemEnergy();
  // doc["frequency"] = readPzemFrequency();
  // doc["pf"] = readPzemPF();

  // dummy random data for testing
  doc["voltage"] = random(2200, 2400) / 10.0;
  doc["current"] = random(0, 1000) / 100.0;
  doc["power"] = random(0, 2000) / 1.0;
  doc["energy"] = random(0, 10000) / 1.0  ;
  doc["frequency"] = random(490, 510) / 1.0;
  doc["powerFactor"] = random(0, 100) / 1.0;

  // Serial.println("Broadcasting PZEM data:");
  // serializeJsonPretty(doc, Serial);
  LOG_INFO("Broadcasting PZEM data: Voltage=%.2fV, Current=%.2fA, Power=%.2fW, Energy=%.2fWh, Frequency=%.2fHz, PF=%.2f",
           doc["voltage"].as<float>(),
           doc["current"].as<float>(),
           doc["power"].as<float>(),
           doc["energy"].as<float>(),
           doc["frequency"].as<float>(),
           doc["pf"].as<float>());

           
  broadcast("pzem:update", doc);
}

