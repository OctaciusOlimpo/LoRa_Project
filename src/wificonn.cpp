#include "wificonn.h"

WiFiConn::WiFiConn(String ssid, String password)
{
    this->ssid = ssid;
    this->password = password;
}

void WiFiConn::connect()
{
    Serial.print("[master] Connecting to ");
    Serial.println(this->ssid);
    WiFi.begin(this->ssid, this->password);
    
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(2000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("[master] WiFi connected.");
    Serial.print("[master] IP address: "); Serial.println(WiFi.localIP());
}