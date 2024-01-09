#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

extern String currentSSID;
extern String currentPassword;
extern String currentURL;
extern String currentIDNode;
extern String currentAPIKey;

extern int nodeNumber;

extern std::vector<String> nodeIDs; 

void setupWebServer();
void setupAPMaster();
void setupAPSlave();

extern WiFiClient client;
extern AsyncWebServer server;

std::vector<String> scanAndCreateNodeIDs();