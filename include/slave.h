#ifndef SLAVE_H
#define SLAVE_H

#include "main.h"

void setupSlave();
void loopSlave();
void readData();

extern int count;

extern String humidity;
extern String temperature;

extern DHTesp dht;

#endif // SLAVE_H
