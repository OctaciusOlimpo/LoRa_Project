#pragma once

#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>

#include "wificonn.h"

#define SCK     5  // GPIO5  SCK
#define MISO    19 // GPIO19 MISO
#define MOSI    27 // GPIO27 MOSI
#define SS      18 // GPIO18 CS
#define RST     14 // GPIO14 RESET
#define DI00    26 // GPIO26 IRQ (Interrupt Request)
//Fim das Definições de Pinagens

#define BAND    915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

class LoRaConn
{
    private:
        // Arrays para armazenar os valores disponíveis
        const long bandwidthOptions[10] = {7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000};
        const int codingRateOptions[4] = {5, 6, 7, 8};
        const int spreadingFactorOptions[7] = {6, 7, 8, 9, 10, 11, 12};
        const int txPowerOptions[7] = {2, 5, 8, 11, 14, 17, 20};
        const bool paBoostOptions[2] = {false, true};  // Desativado, ativado Desativado, ativado

    public:
        //Construtor da classe 
        LoRaConn();
        
        void connect();
        void reconnect(int bandwidthRef, int codingRateRef, int spreadingFactorRef, int txPowerRef, int enablePabootsRef);
};