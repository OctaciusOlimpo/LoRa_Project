// #include "internetStuff.h"

// internetModule::internetModule()
// {
//     //Setup WiFi
//     Serial.println("[internetStuff] Starting Wi-Fi...");
//     connected.wifi = DISCONNECTED;
//     connectionToString[DISCONNECTED] = "Disconnected";
//     connectionToString[CONNECTED] = "Connected";

//     wifiMulti.addAP("Rorschach", "espectral");
//     esp_wifi_set_ps(WIFI_PS_NONE);
//     xTaskCreatePinnedToCore(taskDispatcher, "internetStuffTask", 8192, NULL, 1, NULL, 0);
// }

// void internetModule::controller(void)
// {
//     esp_task_wdt_init(120, true); //120 segundos, modo pânico ativado, reseta esp inteiro caso wdt seja concluído
//     esp_task_wdt_add(NULL);  // Adiciona o wdt nessa task
//     connection state = DISCONNECTED;

//     while(true)
//     {
//         switch (state)
//         {
//             case DISCONNECTED:
//                 Serial.println("[internetStuff] Trying to establish connection...");
//                 xQueueSend(wifiStatusQueue, (void *) &state, 0);
                
//                 while (wifiMulti.run() != WL_CONNECTED) 
//                 {  
//                     vTaskDelay(15000/portTICK_PERIOD_MS);
//                     Serial.println(".");
//                 }
                
//                 Serial.println("\n[internetStuff] Connected to Wi-Fi. SSID: " + WiFi.SSID());
//                 Serial.print("[internetStuff] IP address: " + WiFi.localIP()); 
                
//                 state = CONNECTED;
//                 xQueueSend(wifiStatusQueue, (void *) &state, 0);
//                 break;
            
//             case CONNECTED:
//                 esp_task_wdt_reset();
//                 vTaskDelay(500 / portTICK_PERIOD_MS);
//                 if (WiFi.status() != WL_CONNECTED)
//                 {
//                     state = DISCONNECTED;
//                 }
//                 break;
//         }
//     }
// }

// bool internetModule::wifiConnected()
// {
//     connection state;
//     if(xQueueReceive(wifiStatusQueue, (void *) &state, 0))
//     {
//         if(state)
//         {
//             connected.wifi = CONNECTED;
//         }
//         else  
//         {
//             connected.wifi = DISCONNECTED;
//         }
//     }

//     return connected.wifi;
// }