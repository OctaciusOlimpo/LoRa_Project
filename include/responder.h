#ifndef RESPONDER_H
#define RESPONDER_H

#include <DHTesp.h>
#include "main.h"
#include "loraconn.h"

void setupResponder();
void loopResponder();
void readData();
void configurableLoRa();

#endif //RESPONDER_H
