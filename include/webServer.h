#pragma once

#include "main.h"

//Webserver global variables.
extern String currentSSID;
extern String currentPassword;
extern String currentURL;
extern String currentID;
extern String currentAPIKey;

extern bool currentEnableConfig;
extern int currentBandwidth;
extern int currentCodingRate;
extern int currentSpreadingFactor;
extern int currentTxPower;
extern bool currentEnablePaboost;

extern int numNodes;
extern int nodeNumber;

extern std::vector<String> nodeIDs;
//End of webserver global variables.

//Prototype webserver functions.
void setupAPController();
void setupAPResponder();
bool setupPreferences();
bool setupAP();

std::vector<String> scanAndCreateNodeIDs();
//End prototype of functions.
