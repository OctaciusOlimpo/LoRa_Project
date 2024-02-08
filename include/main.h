#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <SSD1306.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#include "webserver.h"
#include "controller.h"
#include "responder.h"
#include "wificonn.h"
#include "loraconn.h"

//Definições de Pinagens
#define DHTPIN  0

#define OLED    16
#define SDA     4
#define SCL     15

// #define SCK     5  // GPIO5  SCK
// #define MISO    19 // GPIO19 MISO
// #define MOSI    27 // GPIO27 MOSI
// #define SS      18 // GPIO18 CS
// #define RST     14 // GPIO14 RESET
// #define DI00    26 // GPIO26 IRQ (Interrupt Request)
// //Fim das Definições de Pinagens

// #define BAND    915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

extern std::vector<String> nodeIDs; 

//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "setdata=";

//Variável para controlar o display
extern SSD1306 display;

// //Arrays para armazenar os valores disponíveis
// extern const long bandwidthOptions[];
// extern const int codingRateOptions[];
// extern const int spreadingFactorOptions[];
// extern const int txPowerOptions[];
// extern const bool paBoostOptions[];  //Desativado, ativado

// Declare as funções utilizadas em main.cpp
void setupDisplay();

extern LoRaConn* loraConn;

#endif // MAIN_H