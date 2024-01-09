#pragma once

#include "main.h"

extern String currentSSID;
extern String currentPassword;
extern String currentURL;
extern String currentIDNode;
extern String currentAPIKey;

extern String nodeID;
extern int numNodes;

extern int nodeNumber;

void setupWebServer();
void setupAPMaster();
void setupAPSlave();

extern WiFiClient client;
extern AsyncWebServer server;

std::vector<String> scanAndCreateNodeIDs();