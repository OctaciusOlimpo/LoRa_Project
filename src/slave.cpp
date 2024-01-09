#include "slave.h"

String humidity = "";
String temperature = "";

const int NUM_SAMPLES = 10;
float humiditySamples[NUM_SAMPLES];
float temperatureSamples[NUM_SAMPLES];
int sampleIndex = 0;

DHTesp dht;

void setupSlave()
{
  //Chama a configuração inicial do display
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();
  display.clear();
  Serial.println("[slave] Slave esperando...");
  display.drawString(0, 0, "Slave esperando...");
  display.display();

  dht.setup(0, DHTesp::DHT11); // Connect DHT sensor to GPIO 0

  // Inicializa os arrays com um valor que representa uma amostra não coletada
  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    humiditySamples[i] = -1;
    temperatureSamples[i] = -1;
  }
}

void loopSlave()
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

    if(std::find(nodeIDs.begin(), nodeIDs.end(), received) != nodeIDs.end())
    {
      //Simula a leitura dos dados
      readData();
      String data = received + "/" + temperature + "&" + humidity;
      Serial.println("[slave] Criando pacote para envio");
      //Cria o pacote para envio
      LoRa.beginPacket();
      LoRa.print(SETDATA + data);
      //Finaliza e envia o pacote
      LoRa.endPacket();
      //Mostra no display
      display.clear();
      Serial.println("[slave] Enviou: " + String(data));
      display.drawString(0, 0, "Enviou: " + String(data));
      display.display();
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

  // Atualiza os arrays de amostras
  humiditySamples[sampleIndex] = currentHumidity;
  temperatureSamples[sampleIndex] = currentTemperature;

  // Avança para o próximo índice de amostra circular
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

  // Calcula a média móvel
  float averageHumidity = 0;
  float averageTemperature = 0;
  int count = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    // Soma apenas as amostras válidas
    if (humiditySamples[i] != -1) 
    {  // Supondo que -1 seja um valor de inicialização para indicar uma amostra não válida
      averageHumidity += humiditySamples[i];
      averageTemperature += temperatureSamples[i];
      count++;
    }
  }

  if (count > 0) 
  {  // Evita divisão por zero
    averageHumidity /= count;
    averageTemperature /= count;
  }

  // Atualiza as variáveis globais (pode ser modificado conforme sua necessidade)
  humidity = String(averageHumidity);
  temperature = String(averageTemperature);
}

