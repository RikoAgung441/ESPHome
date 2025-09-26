#ifndef PZEM_CONFIG_H
#define PZEM_CONFIG_H
/* code */

#include <PZEM004Tv30.h>
#include "pins_config.h"

extern PZEM004Tv30 pzem;

void initPzem();
float readPzemVoltage();
float readPzemCurrent();
float readPzemPower();
float readPzemEnergy();
float readPzemFrequency();
float readPzemPF();
void broadcastPzemData();




#endif //PZEM_CONFIG_H
