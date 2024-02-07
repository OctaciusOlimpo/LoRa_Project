#pragma once

#include <WiFi.h>

class WiFiConn
{
    private:
        String ssid;
        String password;
   
    public:
        //Construtor da classe 
        WiFiConn(String ssid, String password);
        void connect();
};