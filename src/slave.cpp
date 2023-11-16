#include "slave.h"

//Contador que irá servir como o dados que o Slave irá enviar
int count = 0;

void setupSlave(){
    Serial.begin(115200);
    //Chama a configuração inicial do display
    setupDisplay();
    //Chama a configuração inicial do LoRa
    setupLoRa();
    display.clear();
    display.drawString(0, 0, "Slave esperando...");
    display.display();
}

void loopSlave(){
  //Tenta ler o pacote
  int packetSize = LoRa.parsePacket();

  //Verifica se o pacote possui a quantidade de caracteres que esperamos
  if (packetSize == GETDATA[1].length()){
    String received = "";

    //Armazena os dados do pacote em uma string
    while(LoRa.available()){
      received += (char) LoRa.read();
    }

    if(received == "ID1"){
      //Simula a leitura dos dados
      String data = readData();
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
String readData(){
  return String(count++);
}

