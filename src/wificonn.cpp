#include "wificonn.h"

WiFiConn::WiFiConn(String ssid, String password)
{
    this->ssid = ssid;
    this->password = password;
    
    Serial.println("[wificonn] Constructor wificonn started!");
    Serial.print("[wificonn] Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    connected.wifi = DISCONNECTED;
    
    connectionToString[DISCONNECTED] = "Disconnected";
    connectionToString[CONNECTED] = "Connected";
    
    // Crie uma tarefa para lidar com a conexão Wi-Fi
    xTaskCreatePinnedToCore(controllerDispatcher, "WifiTask", 7300, NULL, 1, NULL, 0); //controllerDispatcher
}

void WiFiConn::controller()
{
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
                Serial.println("[wifitask] WiFi connected.");
                Serial.print("[wifitask] IP address: "); Serial.println(WiFi.localIP());
                state = CONNECTED;
            break;

            case CONNECTED:
                xQueueSend(wifiStatusQueue, (void *) &state, 0);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                
                if(this->ssid != WiFi.SSID() || this->password != WiFi.psk())
                {
                    Serial.println("[wifitask] SSID ou senha alterados. Reiniciando conexão...");
                    // Reinicia a conexão Wi-Fi com os novos parâmetros
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

        // monitorTaskStack(); 
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
  // Obtenha o uso de memória da stack da task
  uint32_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

  // Converta de palavras para bytes
  uint32_t stackHighWaterMarkBytes = stackHighWaterMark * sizeof(StackType_t);

  // Imprima o uso de memória da stack na porta serial
  Serial.print("Stack High Watermark (bytes): ");
  Serial.println(stackHighWaterMarkBytes);

  // Aguarde um tempo antes de verificar novamente (opcional)
  delay(1000);
}