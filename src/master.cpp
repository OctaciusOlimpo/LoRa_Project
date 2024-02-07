#include "master.h"

//Intervalo entre os envios
#define INTERVAL 1000
#define MAX_DISPLAY_LINES 4

// Informações do Broker MQTT
const char* mqtt_server = "192.168.0.176";
const int mqtt_port = 1883;
const char* mqtt_user = "esp32-gateway-1";
const char* mqtt_password = "wonder";

WiFiClient espClient;
PubSubClient clientPubSub(espClient);

//Tempo do último envio
long lastSendTime = 0;
int id = 0;

int idDisplay;

void reconectarMQTT();
void enviarDadosMQTT(String, String, String, String);

void setupMaster()
{
  //Chama a configuração inicial do display
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  display.clear();
  display.drawString(0, 0, "Master");
  display.display();

  // internetModule& internetController = internetModule::getSingleton();

  //Conexão Wifi
  Serial.print("[master] Connecting to ");
  Serial.println(currentSSID);
  WiFi.begin(currentSSID, currentPassword);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("[master] WiFi connected.");
  Serial.print("[master] IP address: "); Serial.println(WiFi.localIP());
  //Fim conexão Wifi
    
  // Configurar o servidor MQTT
  clientPubSub.setServer(mqtt_server, mqtt_port);

  Serial.print("[master] Number of Slaves: ");
  Serial.println(numNodes);

  while(true)
  {
    // while (!internetController.wifiConnected())
    // {
    //   vTaskDelay(100/portTICK_PERIOD_MS);
    // }

    // Reconectar ao servidor MQTT se necessário
    if (!clientPubSub.connected()) 
    {
      reconectarMQTT();
    }

    //Se passou o tempo definido em INTERVAL desde o último envio
    if (millis() - lastSendTime > INTERVAL)
    {
      //Marcamos o tempo que ocorreu o último envio
      lastSendTime = millis();
      //Envia o pacote para informar ao Slave que queremos receber os dados
      send();
    }

    // Lidar com eventos MQTT
    clientPubSub.loop();

    // // Adicione um pequeno atraso para dar um respiro ao sistema
    // delay(100);

    //Verificamos se há pacotes para recebermos
    receive();
  }
}

void loopMaster()
{
  vTaskDelete(NULL);
}

void send()
{
  //Inicializa o pacote
  LoRa.beginPacket();
  //Envia o que está contido em "nodeIDs"
  //Serial.print("[master] "); Serial.println(nodeIDs[id]); 
  LoRa.print(nodeIDs[id]);
  //Finaliza e envia o pacote
  LoRa.endPacket();
}

void receive()
{
  //Tentamos ler o pacote
  int packetSize = LoRa.parsePacket();

  //Verificamos se o pacote tem o tamanho mínimo de caracteres que esperamos
  if (packetSize > SETDATA.length())
  {
    String received = "";
    //Armazena os dados do pacote em uma string
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }
    //Verifica se a string possui o que está contido em "SETDATA"
    int index = received.indexOf(SETDATA);
    if(index >= 0)
    {
      //Recuperamos a string que está após o "SETDATA",
      //que no caso serão os dados de nosso interesse
      String data = received.substring(SETDATA.length());
      //Tempo que demorou para o Master criar o pacote, enviar o pacote,
      //o Slave receber, fazer a leitura, criar um novo pacote, enviá-lo
      //e o Master receber e ler
      String waiting = String(millis() - lastSendTime);
      //Mostra no display os dados e o tempo que a operação demorou
      //display.clear();
      
      int pos1 = data.indexOf('/');   
      int pos2 = data.indexOf('&', pos1 + 1);
      int pos3 = data.indexOf('&', pos2 + 1);

      String readingID = data.substring(0, pos1);
      String temperatureRef = data.substring(pos1 + 1, pos2);
      String humidityRef = data.substring(pos2 + 1, pos3);
      String rssiRef = data.substring(pos3 + 1, data.length());

      Serial.println("[master] ID" + String(id) + " " + readingID);
      if(("ID" + String(id)) == readingID)
      { 
        // sendToAPI(readingID, temperatureRef, humidityRef);  
        // Enviar dados via MQTT
        enviarDadosMQTT(readingID, temperatureRef, humidityRef, rssiRef);

        if(idDisplay <= MAX_DISPLAY_LINES)
        {
          display.clear();
          // Calcula a posição com base no índice
          int displayOffset = idDisplay * 10; // ou qualquer valor apropriado de acordo com o espaçamento desejado
          display.drawString(0, displayOffset, "Recebeu: " + data);
          display.display();
        }
        else
        {
          // Se atingir o limite, retorna para a primeira linha
          idDisplay = 0;
        }
        
        idDisplay++;
        //                 C  L
        display.drawString(0, 50, "Tempo: " + waiting + "ms");
        display.display();
      }
    }
  }    
}

// void sendToAPI(String idAP, String temperatureAP, String humidityAP)
// {
//   // Converta a String para const char*
//   const char* host = currentURL.c_str();

//   if (client.connect(host, 80)) // "184.106.153.149" or api.thingspeak.com
//   {
//     String postStr = currentAPIKey;
  
//     Serial.print("[master] "); Serial.print(idAP); 
//     Serial.print(" "); Serial.print(temperatureAP); Serial.print(" "); Serial.print(humidityAP); 
//     Serial.println(); 

//     postStr += "&field" + String((id+1)*2 - 1) + "=";
//     Serial.print("[master] Indice AP "); Serial.print(String((id+1)*2 - 2)); 
//     Serial.print(" "); Serial.println(String((id+1)*2 - 1)); 
//     postStr += String(temperatureAP);
//     postStr += "&field" + String((id+2)*2 - 2) + "=";
//     postStr += String(humidityAP);

//     postStr += "\r\n\r\n\r\n\r\n";

//     client.print("POST /update HTTP/1.1\n");
//     client.print("Host: api.thingspeak.com\n");
//     client.print("Connection: close\n");
//     client.print("X-THINGSPEAKAPIKEY: " + currentAPIKey + "\n");
//     client.print("Content-Type: application/x-www-form-urlencoded\n");
//     client.print("Content-Length: ");
//     client.print(postStr.length());
//     client.print("\n\n");
//     client.print(postStr);

//     // Mova para o próximo escravo usando circular
//     id = (id + 1) % numNodes;

//   }    
// }

void reconectarMQTT() 
{
  while (!clientPubSub.connected()) 
  {
    Serial.println("[master] Conectando ao servidor MQTT...");
    if (clientPubSub.connect("ESP32Client", mqtt_user, mqtt_password)) 
    {
      Serial.println("[master] Conectado ao servidor MQTT");
    } 
    else 
    {
      Serial.print("[master] Falha na conexão, rc=");
      Serial.println(clientPubSub.state());
      Serial.println("[master] Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void enviarDadosMQTT(String idRef, String temperaturaRef, String umidadeRef, String rssiRef) 
{
  // Criar um objeto JSON
  JsonDocument doc; // Defina o tamanho conforme necessário
  
  // Adicionar os valores ao objeto JSON
  doc["id"] = idRef;
  doc["temperature"] = temperaturaRef.toFloat(); // Converter para float
  doc["humidity"] = umidadeRef.toFloat(); // Converter para float
  doc["rssi"] = rssiRef.toFloat();

  // Serializar o objeto JSON em uma string
  String mensagem;
  serializeJson(doc, mensagem);

  // Publicar a mensagem MQTT
  clientPubSub.publish("teste", mensagem.c_str());

  // Mova para o próximo escravo usando circular
  id = (id + 1) % numNodes;
}
