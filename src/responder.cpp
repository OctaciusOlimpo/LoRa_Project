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
  //Calls the initial Web Server configuration.
  setupAPResponder();
 
  //Calls the initial display configuration.
  setupDisplay();
 
  //Call initial LoRa configuration.
  loraConn->connect();
  
  display.clear();
  Serial.println("[responder] Responder waiting...");
  display.drawString(0, 0, "Responder waiting...");
  display.display();

  dht.setup(21, DHTesp::DHT11); //Connect DHT sensor to GPIO 21.
  nodeID = "ID"+ String(currentID.substring(4).toInt());

  //Initializes arrays with a value that represents an uncollected sample.
  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    humiditySamples[i] = -1;
    temperatureSamples[i] = -1;
    rssiSamples[i] = -1;
  }
}

void loopResponder()
{
  //Try to read the package.
  int packetSize = LoRa.parsePacket();
  
  //Checks if the package has the number of characters we expect.
  if (packetSize > 0)
  {
    String received = "";

    //Stores package data in a string.
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }

    Serial.print("[responder] "); Serial.println(received + ".");

    if(received == nodeID)
    {
      //Read the data.
      readData();
      String data = nodeID + "/" + temperature + "&" + humidity + "&" + rssi;
      Serial.println("[responder] Creating sensor data package for sending.");
      
      //Create the package for shipping.
      LoRa.beginPacket();
      LoRa.print(SETDATA + data);
      
      LoRa.endPacket();
      //Finalize and send the package.

      //Shows on display.
      display.clear();
      Serial.println("[responder] Sent: " + String(data) + ".");
      
      display.drawString(0, 0, "NODE: " + nodeID + ".");
      
      display.drawString(0, 10, "Sent: " + String(data) + ".");
      display.drawString(0, 20, "Temperature: " + String(temperature) + "°C.");
      display.drawString(0, 30, "Humidity: " + String(humidity) + "%.");
      display.drawString(0, 40, "RSSI: " + String(rssi) + "dBm.");
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

//Function where the data you want to send is read.
//It could be the value read by a sensor, for example.
void readData() 
{
  //Reads data from the DHT11 sensor.
  delay(dht.getMinimumSamplingPeriod());
  float currentHumidity = dht.getHumidity();
  float currentTemperature = dht.getTemperature();
  float currentRssi = LoRa.packetRssi();

  //Update sample arrays.
  humiditySamples[sampleIndex] = currentHumidity;
  temperatureSamples[sampleIndex] = currentTemperature;
  rssiSamples[sampleIndex] = currentRssi;

  //Advance to the next circular sample index.
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

  //Calculates the moving average.
  float averageHumidity = 0;
  float averageTemperature = 0;
  float averageRssi = 0;

  int count = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) 
  {
    //Sums only valid samples.
    if (humiditySamples[i] != -1) 
    { //Assuming -1 is an initialization value to indicate an invalid sample.
      averageHumidity += humiditySamples[i];
      averageTemperature += temperatureSamples[i];
      averageRssi += rssiSamples[i];

      count++;
    }
  }

  if (count > 0) 
  { //Avoid division by zero.
    averageHumidity /= count;
    averageTemperature /= count;
    averageRssi /= count;
  }

  //Updates global variables (can be modified as needed).
  humidity = String(averageHumidity);
  temperature = String(averageTemperature);
  rssi = String(averageRssi);
}