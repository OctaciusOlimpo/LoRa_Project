#pragma once

#include <WiFi.h>
#include <map>

void monitorTaskStack();

class WiFiConn
{
    private:
        String ssid;
        String password;

        //Construtor da classe 
        WiFiConn(String ssid, String password);

        void controller(void);

        //Método estático para despachar a conexão
        static void controllerDispatcher(void * params) 
        {
            WiFiConn& instance = getInstance();
            instance.controller();
        }

        QueueHandle_t wifiStatusQueue = xQueueCreate(1, sizeof(connection));

    public:

        // Método estático para obter a instância única da classe
        static WiFiConn& getInstance(String ssid = "", String password = "") 
        {
            // Garante que apenas uma instância será criada
            static WiFiConn instance(ssid, password);

            // Atualiza o SSID e a senha se forem fornecidos
            if (ssid != "" && password != "") 
            {
                instance.ssid = ssid;
                instance.password = password;
            }

            return instance;
        }
        
        // Método para atualizar SSID e senha
        void updateCredentials(String ssid, String password);

        bool wifiConnected();
       
        enum connection {DISCONNECTED, CONNECTED};
        struct connectivity
        {
            connection wifi = DISCONNECTED;
            connection lora = DISCONNECTED;
        }connected;

        std::map<connection, String> connectionToString;
};