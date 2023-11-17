
#include <WiFi.h>

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h"  

// Pin definetion of WIFI LoRa 32
// HelTec AutoMation 2017 support@heltec.cn 
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define BAND 915E6

String apiKey = "5QYIV3TI55EZ12B0";
const char *ssid = "Greenv";
const char *password = "AL@Greenv23*";
const char* server = "api.thingspeak.com";

WiFiClient client;

int rssi;
String loRaMessage;
String temperature;
String humidity;
String readingID;

SSD1306 display(0x3c, 4, 15);


String processor(const String& var)
{
  if(var == "TEMPERATURE")
  {
    return temperature;
  }
  else if(var == "HUMIDITY")
  {
    return humidity;
  }
  else if(var == "RRSI")
  {
    return String(rssi);
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  int counter;
  
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);

  delay(1500);
  display.clear();
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);
  
  while(!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if(counter == 10)
  {
    Serial.println("Starting LoRa failed!");
  }

  LoRa.enableCrc();
  display.drawString(0, 0, "LoRa Initial success!");
  display.drawString(0, 10, "Wait for incomm data...");
  display.display();
  delay(2000);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      Serial.print("Lora packet received: ");
      while (LoRa.available())    // Read packet
    {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      
      
      int pos1 = LoRaData.indexOf('/');   
      int pos2 = LoRaData.indexOf('&');   
      readingID = LoRaData.substring(0, pos1);                   // Get readingID
      temperature = LoRaData.substring(pos1 +1, pos2);           // Get temperature
      humidity = LoRaData.substring(pos2+1, LoRaData.length());  // Get humidity
    }
    
    rssi = LoRa.packetRssi();       // Get RSSI
    Serial.print(" with RSSI ");    
    Serial.println(rssi);
  }
  
    
    if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(readingID);
        postStr += "&field2=";
        postStr += String(temperature);
        postStr += "&field3=";
        postStr += String(humidity);
        postStr += "&field4=";
        postStr += String(rssi);
        postStr += "\r\n\r\n\r\n\r\n";
      
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
  
      }    
      //delay(30000);
}


