// #pragma once

// #include <WiFiMulti.h>
// #include <esp_wifi.h>
// #include <esp_task_wdt.h>
// #include <map>
// #include <esp_log.h>

// #include "main.h"

// class internetModule{
//     private:
//         internetModule(); //Construtor é privado (SingleTon)
        
//         QueueHandle_t wifiStatusQueue = xQueueCreate(1, sizeof(connection));

//         void controller(void);
//         static void taskDispatcher(void * params)
//         {
//             internetModule& instance = getSingleton();
//             instance.controller();
//         }

//     public:
//         //Garantia de que o singleton seja a única instancia recebida
//         static internetModule& getSingleton()
//         {
//             static internetModule singleton;
//             return singleton;
//         }

//         //Delete a cópia do construtor e garanta uma atribuição ao operador para prevenir duplicação
//         internetModule(internetModule&) = delete;
//         void operator = (const internetModule&) = delete;

//         ~internetModule(){}

//         enum connection {DISCONNECTED, CONNECTED};

//         struct connectivity
//         {
//             connection wifi = DISCONNECTED;
//             connection lora = DISCONNECTED;
//         }connected;

//         std::map<connection, String> connectionToString;

//         WiFiMulti wifiMulti;

//         bool wifiConnected();
// };