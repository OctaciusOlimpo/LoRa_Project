#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "main.h"
#include "wificonn.h"
#include "loraconn.h"
#include "webserver.h"

void setupController();
void loopController();
void send();
void receive();
void sendToAPI(String, String, String);
void loraConfigurable();

extern String loraMessage;
extern String readingID;
extern WiFiConn* wifiConn;

#endif //CONTROLLER_H
