#pragma once

#include "main.h"

extern String currentSSID;
extern String currentPassword;
extern String currentURL;
extern String currentID;
extern String currentAPIKey;

extern int numNodes;
extern int nodeNumber;

void setupAPController();
void setupAPResponder();

extern std::vector<String> nodeIDs;
std::vector<String> scanAndCreateNodeIDs();