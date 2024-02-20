#include "main.h"

SSD1306 display(0x3c, 4, 15);

LoRaConn* loraConn = new LoRaConn();

void logo()
{
  display.clear();
  display.drawXbm(0,5,logo_width, logo_height, logo_bits);
  display.display();
}

void setupDisplay()
{
  //The state of GPIO16 is used to control the OLED display.
  pinMode(16, OUTPUT);
  
  //Resets OLED display settings.
  digitalWrite(16, LOW);
  
  //For the OLED to remain on, GPIO16 must remain HIGH
  //It must be HIGH before calling display.init() and making other settings,
  //do not reverse the order.
  digitalWrite(16, HIGH);

  //Display Settings.
  display.init();
  //display.setContrast(255, 241, 64);
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  logo();
  delay(1500);
  display.clear();

}

void setup()
{
  Serial.begin(115200);

  #ifdef CONTROLLER
    //Initial "Controller" Setup.
    setupController();
  #else
    //Initial "Responder" Configuration.
    setupResponder();
  #endif
}

void loop()
{
  #ifdef CONTROLLER
    //Main "Controller" loop.
    loopController();
  #else
    //"Responder" main loop.
    loopResponder();
  #endif
}