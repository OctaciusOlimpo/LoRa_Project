#include "wificonn.h"

WiFiConn::WiFiConn(String ssid, String password)
{
    this->ssid = ssid;
    this->password = password;
    
    //Serial.println("[wificonn] Constructor wificonn started!");
    Serial.print("[wificonn] Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    connected.wifi = DISCONNECTED;
    
    connectionToString[DISCONNECTED] = "Disconnected";
    connectionToString[CONNECTED] = "Connected";
    
    esp_wifi_set_ps(WIFI_PS_NONE);

    //Create a task to handle the Wi-Fi connection
    xTaskCreatePinnedToCore(controllerDispatcher, "WifiTask", 7300, NULL, 1, NULL, 0); //controllerDispatcher
}

void WiFiConn::controller()
{
    esp_task_wdt_init(120, true); //120 seconds, panic mode = true, reset entire esp
    esp_task_wdt_add(NULL);

    connection state = DISCONNECTED;

    while(1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);

        switch(state)
        {
            case DISCONNECTED:
                Serial.println("[wifitask] Connecting to WiFi...");
                
                xQueueSend(wifiStatusQueue, (void *) &state, 0);

                while(WiFi.status() != WL_CONNECTED)
                {
                    vTaskDelay(5000/portTICK_PERIOD_MS);
                    Serial.print(".");
                }
                
                Serial.println("");
                Serial.println("[wificonn] WiFi connected.");
                Serial.print("[wificonn] IP address: "); Serial.println(WiFi.localIP());
                state = CONNECTED;
            break;

            case CONNECTED:
                xQueueSend(wifiStatusQueue, (void *) &state, 0);
                
                esp_task_wdt_reset();
                
                vTaskDelay(500 / portTICK_PERIOD_MS);
                
                if(this->ssid != WiFi.SSID() || this->password != WiFi.psk())
                {
                    Serial.println("[wificonn] SSID ou senha alterados. Reiniciando conexão...");
                    
                    //Restarts the Wi-Fi connection with the new parameters
                    WiFi.disconnect(true);
                    WiFi.begin(this->ssid, this->password);
                    state = DISCONNECTED;
                }
                
                if (WiFi.status() != WL_CONNECTED)
                {
                    state = DISCONNECTED;
                }
            break;
        }

        //Uncomment to inform task size
        //monitorTaskStack(); 
    }
}

bool WiFiConn::wifiConnected()
{
    connection state;
    if(xQueueReceive(wifiStatusQueue, (void *) &state, 0))
    {
        if(state)
        {
            connected.wifi = CONNECTED;
        }
        else
        {
            connected.wifi = DISCONNECTED;
        }
    }

    return connected.wifi;
}

void WiFiConn::updateCredentials(String ssid, String password) 
{
    this->ssid = ssid;
    this->password = password;
}

void monitorTaskStack() 
{
  //Get the memory usage of the task stack
  uint32_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

  //Convert from words to bytes
  uint32_t stackHighWaterMarkBytes = stackHighWaterMark * sizeof(StackType_t);

  //Print stack memory usage on the serial port
  Serial.print("Stack High Watermark (bytes): ");
  Serial.println(stackHighWaterMarkBytes);

  //Wait a while before checking again (optional)
  delay(1000);
}