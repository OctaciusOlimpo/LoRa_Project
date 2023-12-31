#include "master.h"

//Intervalo entre os envios
#define INTERVAL 2000

//Tempo do último envio
long lastSendTime = 0;
int id = 0;

bool displayLineToggle = false;

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
}

void loopMaster(){
  //Se passou o tempo definido em INTERVAL desde o último envio
  if (millis() - lastSendTime > INTERVAL){
    //Marcamos o tempo que ocorreu o último envio
    lastSendTime = millis();
    //Envia o pacote para informar ao Slave que queremos receber os dados
    send(id);

    if(id == 0)
    {
      id++;
    }
    else
    {
      id = 0;
    }
  }

  //Verificamos se há pacotes para recebermos
  receive();
}

void send(int id){
  //Inicializa o pacote
  LoRa.beginPacket();
  //Envia o que está contido em "GETDATA"
  LoRa.print(GETDATA[id]);
  //Finaliza e envia o pacote
  LoRa.endPacket();
}

void receive(){
  //Tentamos ler o pacote
  int packetSize = LoRa.parsePacket();
  
  //Verificamos se o pacote tem o tamanho mínimo de caracteres que esperamos
  if (packetSize > SETDATA.length()){
    String received = "";
    //Armazena os dados do pacote em uma string
    while(LoRa.available()){
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

      sendToAPI(readingID, temperatureRef, humidityRef);

      if(displayLineToggle)
      {
        display.clear();
        display.drawString(0, 0, "Recebeu: " + data);
      }
      else
      {
        display.drawString(0, 10, "Recebeu: " + data);
      }
      
      //                 C  L
      display.drawString(0, 20, "Tempo: " + waiting + "ms");
      display.display();
    
      displayLineToggle = !displayLineToggle;
    }
  }
}

void sendToAPI(String readingID, String temperatureAP, String humidityAP)
{
  // Converta a String para const char*
  const char* host = currentURL.c_str();

  if (client.connect(host, 80)) // "184.106.153.149" or api.thingspeak.com
      {
          String postStr = currentAPIKey;
          if(readingID == "ID0")
          {
      
            Serial.print("[master] "); Serial.print(readingID); Serial.print(" "); Serial.print(temperatureAP);
            Serial.println(); 

            postStr += "&field1=";
            postStr += String(temperatureAP);
            postStr += "&field2=";
            postStr += String(humidityAP);
          }
          else if(readingID == "ID1")
          {
      
            Serial.print("[master] "); Serial.print(readingID); Serial.print(" "); Serial.print(temperatureAP);
            Serial.println(); 

            postStr += "&field3=";
            postStr += String(temperatureAP);
            postStr += "&field4=";
            postStr += String(humidityAP);
            postStr += "\r\n\r\n\r\n\r\n";
          }
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
