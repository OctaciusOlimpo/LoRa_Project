#pragma once

#include "main.h"

// Variáveis globais webserver
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
// Fim das variáveis globais webserver

// Protótipo funções webserver
void setupAPController();
void setupAPResponder();
std::vector<String> scanAndCreateNodeIDs();
//Fim protótipo das funções 
