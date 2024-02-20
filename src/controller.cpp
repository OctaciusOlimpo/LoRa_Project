#include "controller.h"

//Interval between sending.
#define INTERVAL 1000
#define MAX_DISPLAY_LINES 4

//MQTT Broker information.
const char* mqtt_server = "192.168.0.176";
const int mqtt_port = 1883;
const char* mqtt_user = "esp32-gateway-1";
const char* mqtt_password = "wonder";

//Client to HTTP Post.
WiFiClient clientHttp;

//Client to MQTT Pub Sub.
WiFiClient clientMqtt;
PubSubClient clientPubSub(clientMqtt);

//Last sending time.
long lastSendTime = 0;
int id = 0;

int idDisplay;

void reconectarMQTT();
void enviarDadosMQTT(String, String, String, String);

void setupController()
{
  //Calls the initial display configuration.
  setupDisplay();
  
  //Calls initial LoRa configuration.
  loraConn->connect();
  
  //Calls web server configuration.
  setupAPController();

  display.clear();
  display.drawString(0, 0, "Controller waiting...");
  display.display();

  //Global declaration of the WiFiConn class instance.
  WiFiConn& wifi = WiFiConn::getInstance(currentSSID, currentPassword);

  while(!wifi.wifiConnected()){}

  //MQTT server settings.
  clientPubSub.setServer(mqtt_server, mqtt_port);

  Serial.print("[controller] Number of Responders: ");
  Serial.println(numNodes);

  while(true)
  {
    //Reconnect to MQTT server if necessary.
    if (!clientPubSub.connected() && wifi.wifiConnected()) 
    {
      reconectarMQTT();
    }

    //If the time defined in INTERVAL has passed since the last sending...
    if (millis() - lastSendTime > INTERVAL)
    {
      //We mark the time when the last sending occurred!
      lastSendTime = millis();
      
      //And send the packet to inform the "responder" that we want to receive the data.
      if(!currentEnableConfig)
      {
        send();
      }
      else
      {
        loraConfigurable();
        currentEnableConfig = false;

        loraConn->reconnect(currentBandwidth, currentCodingRate, currentSpreadingFactor, currentTxPower, currentEnablePaboost);
      }
    }

    //Handle MQTT events.
    clientPubSub.loop();

    //We check if there are packages for us to receive.
    receive();
  }
}

void loopController()
{
  vTaskDelete(NULL);
}

void send()
{
  //Initialize the package.
  LoRa.beginPacket();
  
  //Sends what is contained in "nodeIDs".
  LoRa.print(nodeIDs[id]);
  
  //Finalize and send the package.
  LoRa.endPacket();
}

void receive()
{
  //We try to read the package.
  int packetSize = LoRa.parsePacket();

  //We check that the packet has the minimum character size we expect.
  if (packetSize > SETDATA.length())
  {
    String received = "";
    
    //Stores package data in a string.
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }
    
    //Checks if the string has what is contained in "SETDATA".
    int index = received.indexOf(SETDATA);
    if(index >= 0)
    {
      //We retrieve the string after "SETDATA",
      //which in this case will be the data of interest to us.
      String data = received.substring(SETDATA.length());
      
      //Time it took for the "controller" to create the package, send the package,
      //the "responder" receives, reads, creates a new packet, sends it
      //and the "controller" receives and reads.
      String waiting = String(millis() - lastSendTime);
      
      //Shows on the display the data and the time the operation took.
      //display.clear();
      
      int pos1 = data.indexOf('/');   
      int pos2 = data.indexOf('&', pos1 + 1);
      int pos3 = data.indexOf('&', pos2 + 1);

      String readingID = data.substring(0, pos1);
      String temperatureRef = data.substring(pos1 + 1, pos2);
      String humidityRef = data.substring(pos2 + 1, pos3);
      String rssiRef = data.substring(pos3 + 1, data.length());

      //Serial.println("[controller] ID match: ID" + String(id) + " = " + readingID + "?");
      
      if(("ID" + String(id)) == readingID)
      { 
        //Serial.println("[controller] Yes!");

        //Send data via HTTP POST.
        sendToAPI(readingID, temperatureRef, humidityRef);  
       
        //Send data via MQTT.
        enviarDadosMQTT(readingID, temperatureRef, humidityRef, rssiRef);

        if(idDisplay <= MAX_DISPLAY_LINES)
        {
          display.clear();
          
          //Calculates position based on index...
          int displayOffset = idDisplay * 10; //...or any appropriate value as per desired spacing.
          display.drawString(0, displayOffset, "Rx: " + data);
          display.display();
        }
        else
        {
          //If it reaches the limit, it returns to the first line.
          idDisplay = 0;
        }
        
        idDisplay++;
        
        //Note: First number refers to colum and second number refers to line. 
        display.drawString(0, 50, "Time: " + waiting + "ms.");
        display.display();
      
        //Move to next slave using circular index.
        id = (id + 1) % numNodes;
      }
      else
      {
        //Serial.println("[controller] No!");
      }
    }
  }    
}

void sendToAPI(String idAP, String temperatureAP, String humidityAP)
{
  //Convert String to const char*.
  const char* host = currentURL.c_str();

  if (clientHttp.connect(host, 80)) //"184.106.153.149" or api.thingspeak.com.
  {
    String postStr = currentAPIKey;
  
    //Serial.print("[master] "); Serial.print(idAP); 
    //Serial.print(" "); Serial.print(temperatureAP); Serial.print(" "); Serial.print(humidityAP); 
    //Serial.println(); 

    postStr += "&field" + String((id+1)*2 - 1) + "=";
    //Serial.print("[master] Indice AP "); Serial.print(String((id+1)*2 - 2)); 
    //Serial.print(" "); Serial.println(String((id+1)*2 - 1)); 
    postStr += String(temperatureAP);
    postStr += "&field" + String((id+2)*2 - 2) + "=";
    postStr += String(humidityAP);

    postStr += "\r\n\r\n\r\n\r\n";

    clientHttp.print("POST /update HTTP/1.1\n");
    clientHttp.print("Host: api.thingspeak.com\n");
    clientHttp.print("Connection: close\n");
    clientHttp.print("X-THINGSPEAKAPIKEY: " + currentAPIKey + "\n");
    clientHttp.print("Content-Type: application/x-www-form-urlencoded\n");
    clientHttp.print("Content-Length: ");
    clientHttp.print(postStr.length());
    clientHttp.print("\n\n");
    clientHttp.print(postStr);

    //Move to next slave using circular index.
    //id = (id + 1) % numNodes;
  }    
}

void reconectarMQTT() 
{
  while (!clientPubSub.connected()) 
  {
    Serial.println("[controller] Connecting to MQTT server...");

    if (clientPubSub.connect("ESP32Client", mqtt_user, mqtt_password)) 
    {
      Serial.println("[controller] Connected to MQTT server.");
    } 
    else 
    {
      Serial.print("[controller] Connection fail, rc=");
      Serial.println(clientPubSub.state());
      Serial.println("[controller] Trying again in 5 seconds...");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      //delay(5000);
    }
  }
}

void enviarDadosMQTT(String idRef, String temperaturaRef, String umidadeRef, String rssiRef) 
{
  //Create a JSON object.
  JsonDocument doc; //Set the size as needed.
  
  //Add the values ​​to the JSON object.
  doc["id"] = idRef;
  doc["temperature"] = temperaturaRef.toFloat(); //Convert to float.
  doc["humidity"] = umidadeRef.toFloat(); //Convert to float.
  doc["rssi"] = rssiRef.toFloat(); //Convert to float.

  //Serialize JSON object into a string.
  String mensagem;
  serializeJson(doc, mensagem);

  //Publish the MQTT message.
  clientPubSub.publish("teste", mensagem.c_str());

  //Move to next slave using circular index.
  //id = (id + 1) % numNodes;
}

void loraConfigurable()
{
  //Initialize the package.
  LoRa.beginPacket();

  Serial.println("[master] Creating package for LoRa changing.");
  String data = "CF";
  data += String(currentBandwidth) + "&" + String(currentCodingRate) + "&"; 
  data += String(currentSpreadingFactor) + "&" + String(currentTxPower) + "&" + String(currentEnablePaboost);
  
  //Sends what is contained in "data".
  LoRa.print(data);
  //Finalize and send the package.
  LoRa.endPacket();
}