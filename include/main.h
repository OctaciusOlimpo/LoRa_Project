#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306.h>
#include <DHTesp.h> // Click here to get the library: http://librarymanager/All#DHTesp
#include <Ticker.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#include "webServer.h"
#include "master.h"
#include "slave.h"

//Deixe esta linha descomentada para compilar o Master
//Comente ou remova para compilar o Slave
//#define MASTER

#define SCK 5   // GPIO5  SCK
#define MISO 19 // GPIO19 MISO
#define MOSI 27 // GPIO27 MOSI
#define SS 18   // GPIO18 CS
#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

#define BAND 915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

extern std::vector<String> nodeIDs; 

//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "setdata=";

//Variável para controlar o display

extern SSD1306 display;

// Declare as funções utilizadas em main.cpp
void setupDisplay();
void setupLoRa();

#endif // MAIN_H