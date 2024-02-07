#ifndef MASTER_H
#define MASTER_H

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "main.h"

void setupMaster();
void loopMaster();
void send();
void receive();
void sendToAPI(String, String, String);

// extern int rssi;
extern String loraMessage;
extern String readingID;

#endif // MASTER_H
