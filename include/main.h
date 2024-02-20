#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <SSD1306.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#include "images.h"
#include "webserver.h"
#include "controller.h"
#include "responder.h"
#include "wificonn.h"
#include "loraconn.h"

//Pinout Settings.
#define DHTPIN  0

#define OLED    16
#define SDA     4
#define SCL     15

extern std::vector<String> nodeIDs; 

//Constant that the "responder" returns along with the data to the "controller".
const String SETDATA = "setdata=";

//Variable to control the display.
extern SSD1306 display;

//Declare the functions used in main.cpp.
void setupDisplay();

extern LoRaConn* loraConn;

#endif //MAIN_H.