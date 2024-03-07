#include "loraconn.h"

//LoRa initial settings.
LoRaConn::LoRaConn()
{ 
  //Initializes communication.
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);
}

void LoRaConn::connect()
{
  //Initialize LoRa.
  if (!LoRa.begin(BAND))
  {
    Serial.println("[loraconn] Error initializing LoRa!");
    //while (1){}

    Serial.println("[loraconn] Resetting ESP32 in 5 seconds!");
    delay(5000);
    esp_restart();
  }

  //LoRa.setSignalBandwidth(bandwidthOptions[4]);
  //LoRa.setCodingRate4(codingRateOptions[0]);
  //LoRa.setSpreadingFactor(spreadingFactorOptions[0]);
  //LoRa.setTxPower(txPowerOptions[0], paBoostOptions[1]);

  //Activate crc.
  LoRa.enableCrc();
  
  //Activates packet reception.
  LoRa.receive();
}

void LoRaConn::reconnect(int bandwidthRef, int codingRateRef, int spreadingFactorRef, int txPowerRef, int enablePaboostRef)
{
  //Initialize LoRa again with the new settings.
  if (!LoRa.begin(BAND))
  {
    //If it was unable to initialize, a message will appear on the serial monitor.
    Serial.println("[loraconn] Error initializing LoRa!");
    //while (1){}

    Serial.println("[loraconn] Resetting ESP32 in 5 seconds!");
    delay(5000);
    esp_restart();
  }
  
  LoRa.setSignalBandwidth(bandwidthOptions[bandwidthRef]);
  LoRa.setCodingRate4(codingRateOptions[codingRateRef]);
  LoRa.setSpreadingFactor(spreadingFactorOptions[spreadingFactorRef]);
  LoRa.setTxPower(txPowerOptions[txPowerRef], paBoostOptions[enablePaboostRef]);

  Serial.println("[loraconn] Changed successfully!");

  //Activate crc.
  LoRa.enableCrc();
}