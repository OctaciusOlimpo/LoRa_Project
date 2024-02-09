#include "loraconn.h"

//Configurações iniciais do LoRa
LoRaConn::LoRaConn()
{ 
  //Inicializa a comunicação
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);
}

void LoRaConn::connect()
{
  //Inicializa o LoRa
  if (!LoRa.begin(BAND))
  {
    //Se não conseguiu inicializar, mostra uma mensagem no display
    // display.clear();
    Serial.println("[main] Error initializing LoRa!");
    // display.drawString(0, 0, "Error initializing LoRa!");
    // display.display();
    while (1);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
}

void LoRaConn::reconnect(int bandwidthRef, int codingRateRef, int spreadingFactorRef, int txPowerRef, int enablePaboostRef)
{
  //Inicializa o LoRa
  if (!LoRa.begin(BAND))
  {
    //Se não conseguiu inicializar, mostra uma mensagem no display
    Serial.println("[loraconn] Erro ao inicializar o LoRa!");
    while (1){};
  }
  
  LoRa.setSignalBandwidth(bandwidthOptions[bandwidthRef]);
  LoRa.setCodingRate4(codingRateOptions[codingRateRef]);
  LoRa.setSpreadingFactor(spreadingFactorOptions[spreadingFactorRef]);
  LoRa.setTxPower(txPowerOptions[txPowerRef], paBoostOptions[enablePaboostRef]);

  Serial.println("[loraconn] Alterado com Sucesso!");

  //Ativa o crc
  LoRa.enableCrc();
}