#include "slave.h"

//Contador que irá servir como o dados que o Slave irá enviar
int count = 0;

String humidity = "";
String temperature = "";

DHTesp dht;

void setupSlave(){
    //Chama a configuração inicial do display
    setupDisplay();
    //Chama a configuração inicial do LoRa
    setupLoRa();
    display.clear();
    display.drawString(0, 0, "Slave esperando...");
    display.display();

    dht.setup(0, DHTesp::DHT11); // Connect DHT sensor to GPIO 17
}

void loopSlave(){
  //Tenta ler o pacote
  int packetSize = LoRa.parsePacket();

  //Verifica se o pacote possui a quantidade de caracteres que esperamos
  if (packetSize == GETDATA[0].length()){
    String received = "";

    //Armazena os dados do pacote em uma string
    while(LoRa.available()){
      received += (char) LoRa.read();
    }

    if(received == "ID0"){
      //Simula a leitura dos dados
      readData();
      String data = String(GETDATA[0]) + "/" + temperature + "&" + humidity;
      Serial.println("Criando pacote para envio");
      //Cria o pacote para envio
      LoRa.beginPacket();
      LoRa.print(SETDATA + data);
      //Finaliza e envia o pacote
      LoRa.endPacket();
      //Mostra no display
      display.clear();
      display.drawString(0, 0, "Enviou: " + String(data));
      display.display();
    }
  }
}

//Função onde se faz a leitura dos dados que queira enviar
//Poderia ser o valor lido por algum sensor por exemplo
//Aqui vamos enviar apenas um contador para testes
//mas você pode alterar a função para fazer a leitura de algum sensor
void readData(){
  delay(dht.getMinimumSamplingPeriod());

  humidity = (String)dht.getHumidity();
  temperature = (String)dht.getTemperature();
}

