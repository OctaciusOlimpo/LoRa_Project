#ifndef MASTER_H
#define MASTER_H

#include "main.h"

void setupMaster();
void loopMaster();
void send(int);
void receive();
String processor(const String&);
void sendToAPI(String, String, String);

extern int rssi;
extern String loraMessage;
extern String readingID;

#endif // MASTER_H
