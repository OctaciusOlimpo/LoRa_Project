#include "main.h"

SSD1306 display(0x3c, 4, 15);

void setupDisplay()
{
  //O estado do GPIO16 é utilizado para controlar o display OLED
  pinMode(16, OUTPUT);
  //Reseta as configurações do display OLED
  digitalWrite(16, LOW);
  //Para o OLED permanecer ligado, o GPIO16 deve permanecer HIGH
  //Deve estar em HIGH antes de chamar o display.init() e fazer as demais configurações,
  //não inverta a ordem
  digitalWrite(16, HIGH);

  //Configurações do display
  display.init();
  //display.setContrast(255, 241, 64);
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  //display.setBrightness(255);
}

//Configurações iniciais do LoRa
void setupLoRa()
{ 
  //Inicializa a comunicação
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);

  //Inicializa o LoRa
  if (!LoRa.begin(BAND)){
    //Se não conseguiu inicializar, mostra uma mensagem no display
    display.clear();
    Serial.println("[main] Erro ao inicializar o LoRa!");
    display.drawString(0, 0, "Erro ao inicializar o LoRa!");
    display.display();
    while (1);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
}

void setup()
{

  Serial.begin(115200);

  #ifdef MASTER
    // Configuração inicial do Master
    setupAPMaster();
    setupMaster();
  #else
    // Configuração inicial do Slave
    setupAPSlave();
    setupSlave();
  #endif
}

void loop()
{
  #ifdef MASTER
    // Loop principal do Master
    loopMaster();
  #else
    // Loop principal do Slave
    loopSlave();
  #endif
}