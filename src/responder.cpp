#include "responder.h"

String humidity = "";
String temperature = "";
String rssi = "";

const int NUM_SAMPLES = 10;
float humiditySamples[NUM_SAMPLES];
float temperatureSamples[NUM_SAMPLES];
float rssiSamples[NUM_SAMPLES];
int sampleIndex = 0;

String nodeID;

DHTesp dht;

void setupResponder()
{
  //Chama a configuração inicial do Web Server
  setupAPResponder();
 
  //Chama a configuração inicial do display
  setupDisplay();
 
  //Chama a configuração inicial do LoRa
  loraConn->connect();
  
  display.clear();
  Serial.println("[responder] Responder waiting...");
  display.drawString(0, 0, "Responder waiting...");
  display.display();

  dht.setup(21, DHTesp::DHT11); // Connect DHT sensor to GPIO 0
  nodeID = "ID"+ String(currentID.substring(4).toInt());

  // Inicializa os arrays com um valor que representa uma amostra não coletada
  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    humiditySamples[i] = -1;
    temperatureSamples[i] = -1;
    rssiSamples[i] = -1;
  }
}

void loopResponder()
{
  //Tenta ler o pacote
  int packetSize = LoRa.parsePacket();
  //Verifica se o pacote possui a quantidade de caracteres que esperamos
  if (packetSize > 0)
  {
    String received = "";

    //Armazena os dados do pacote em uma string
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }

    Serial.print("[responder] "); Serial.println(received);

    if(received == nodeID)
    {
      //Faz a leitura dos dados
      readData();
      String data = nodeID + "/" + temperature + "&" + humidity + "&" + String(LoRa.packetRssi());
      Serial.println("[responder] Creating sensor data package for sending");
      
      //Cria o pacote para envio
      LoRa.beginPacket();
      LoRa.print(SETDATA + data);
      
      LoRa.endPacket();
      //Finaliza e envia o pacote

      //Mostra no display
      display.clear();
      Serial.println("[responder] Sent: " + String(data) + ".");
      display.drawString(0, 0, "Sent: " + String(data));
      display.display();
    }
    else if(received.indexOf("CF") != -1)
    {

      String data = received.substring(String("CF").length());

      int pos1 = data.indexOf('&');
      int pos2 = data.indexOf('&', pos1 + 1);
      int pos3 = data.indexOf('&', pos2 + 1);
      int pos4 = data.indexOf('&', pos3 + 1);
      int pos5 = data.indexOf('&', pos4 + 1);

      String bandwidthRef = data.substring(0, pos1);
      String codingRateRef = data.substring(pos1 + 1, pos2);
      String spreadingFactorRef = data.substring(pos2 + 1, pos3);
      String txPowerRef = data.substring(pos3 + 1, pos4);
      String enablePABOOSTRef = data.substring(pos4 + 1, data.length());

      Serial.println("[responder] LoRa Data: " + bandwidthRef + " " + codingRateRef + " "+ spreadingFactorRef + " " + txPowerRef + " " + enablePABOOSTRef);
      Serial.println("[responder] LoRa going to change!");

      loraConn->reconnect(bandwidthRef.toInt(), codingRateRef.toInt(), spreadingFactorRef.toInt(), txPowerRef.toInt(), enablePABOOSTRef.toInt());
    }
  }
}

//Função onde se faz a leitura dos dados que queira enviar
//Poderia ser o valor lido por algum sensor por exemplo
//Aqui vamos enviar apenas um contador para testes
//mas você pode alterar a função para fazer a leitura de algum sensor
void readData() 
{
  // Realiza a leitura dos dados do sensor DHT11
  delay(dht.getMinimumSamplingPeriod());
  float currentHumidity = dht.getHumidity();
  float currentTemperature = dht.getTemperature();
  float currentRssi = LoRa.packetRssi();

  // Atualiza os arrays de amostras
  humiditySamples[sampleIndex] = currentHumidity;
  temperatureSamples[sampleIndex] = currentTemperature;
  rssiSamples[sampleIndex] = currentRssi;

  // Avança para o próximo índice de amostra circular
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

  // Calcula a média móvel
  float averageHumidity = 0;
  float averageTemperature = 0;
  float averageRssi = 0;

  int count = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    // Soma apenas as amostras válidas
    if (humiditySamples[i] != -1) 
    {  // Supondo que -1 seja um valor de inicialização para indicar uma amostra não válida
      averageHumidity += humiditySamples[i];
      averageTemperature += temperatureSamples[i];
      averageRssi += rssiSamples[i];

      count++;
    }
  }

  if (count > 0) 
  {  // Evita divisão por zero
    averageHumidity /= count;
    averageTemperature /= count;
    averageRssi /= count;
  }

  // Atualiza as variáveis globais (pode ser modificado conforme sua necessidade)
  humidity = String(averageHumidity);
  temperature = String(averageTemperature);
  rssi = String(averageRssi);
}