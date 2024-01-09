#include "master.h"

//Intervalo entre os envios
#define INTERVAL 2000
#define MAX_DISPLAY_LINES 4

//std::map<String, bool> nodeSentStatus; // Mapa para rastrear o status de envio de cada node

//Tempo do último envio
long lastSendTime = 0;
int id = 0;

int idDisplay;

bool flag;

void setupMaster()
{
  //Chama a configuração inicial do display
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  display.clear();
  display.drawString(0, 0, "Master");
  display.display();

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
    
  Serial.print("[master] Number of Slaves: ");
  Serial.println(numNodes);
}

void loopMaster()
{

  flag = false;

  //Se passou o tempo definido em INTERVAL desde o último envio
  if (millis() - lastSendTime > INTERVAL)
  {
    //Marcamos o tempo que ocorreu o último envio
    lastSendTime = millis();
    //Envia o pacote para informar ao Slave que queremos receber os dados
    send(id);

     // Mova para o próximo escravo usando circular
    id = (id + 1) % numNodes;
  }

  //Verificamos se há pacotes para recebermos
  receive();
}

void send(int id)
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
    if(index >= 0){
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
      int pos2 = data.indexOf('&');

      String readingID = data.substring(0, pos1);
      String temperatureRef = data.substring(pos1 + 1, pos2);
      String humidityRef = data.substring(pos2 + 1, data.length());
      
      while(!flag)
      {
        sendToAPI(readingID, temperatureRef, humidityRef, id);  
      }
      
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

void sendToAPI(String readingID, String temperatureAP, String humidityAP, int id)
{
  // Converta a String para const char*
  const char* host = currentURL.c_str();

  if (client.connect(host, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = currentAPIKey;
  
    Serial.print("[master] "); Serial.print(readingID); 
    Serial.print(" "); Serial.print(temperatureAP); Serial.print(" "); Serial.print(humidityAP); 
    Serial.println(); 

    postStr += "&field" + String((id+1)*2 - 1) + "=";
    Serial.print("[master] Indice AP "); Serial.print(String((id+1)*2 - 2)); 
    Serial.print(" "); Serial.println(String((id+1)*2 - 1)); 
    postStr += String(temperatureAP);
    postStr += "&field" + String((id+2)*2 - 2) + "=";
    postStr += String(humidityAP);

    flag = true;

    postStr += "\r\n\r\n\r\n\r\n";
       
    /*    
      postStr += "&field4=";
      postStr += String(rssi);
      postStr += "\r\n\r\n\r\n\r\n";
    */

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + currentAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

  }    
}
