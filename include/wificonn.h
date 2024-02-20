#pragma once

#include <WiFi.h>
#include <map>
#include <esp_log.h>
#include <esp_task_wdt.h>
#include <esp_wifi.h>

void monitorTaskStack();

class WiFiConn
{
    private:
        String ssid;
        String password;

        //Class constructor.
        WiFiConn(String ssid, String password);

        void controller(void);

        //Static method to dispatch the connection.
        static void controllerDispatcher(void * params) 
        {
            WiFiConn& instance = getInstance();
            instance.controller();
        }

        QueueHandle_t wifiStatusQueue = xQueueCreate(1, sizeof(connection));

    public:

        //Static method to get single instance of class.
        static WiFiConn& getInstance(String ssid = "", String password = "") 
        {
            //Ensures that only one instance will be created.
            static WiFiConn instance(ssid, password);

            //Updates SSID and password if provided.
            if (ssid != "" && password != "") 
            {
                instance.ssid = ssid;
                instance.password = password;
            }

            return instance;
        }
        
        //Method to update SSID and password.
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