#include "main.h"

SSD1306 display(0x3c, 4, 15);

LoRaConn* loraConn = new LoRaConn();

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
}

void setup()
{
  Serial.begin(115200);

  #ifdef CONTROLLER
    // Configuração inicial do Master
    setupController();
  #else
    // Configuração inicial do Slave
    setupAPResponder();
    setupResponder();
  #endif
}

void loop()
{
  #ifdef CONTROLLER
    // Loop principal do Master
    loopController();
  #else
    // Loop principal do Slave
    loopResponder();
  #endif
}