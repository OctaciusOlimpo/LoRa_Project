#include "webserver.h"

Preferences preferences;

const char *ssid = "GATEWAY-AP";
const char *password = "password";
const int serverPort = 80;

int nodeNumber;

#ifdef CONTROLLER
    #ifndef ID
    #define ID "GATEWAY-AP" //Default value if not specified in build flags
    #endif
#else
    #ifndef ID
    #define ID "NODE" //Default value if not specified in build flags
    #endif
#endif

#ifndef WIFISSID
#define WIFISSID "Rorschach" //Default value if not specified in build flags
#endif
#ifndef PASSWORD
#define PASSWORD "espectral" //Default value if not specified in build flags
#endif

String currentSSID = String(WIFISSID);
String currentPassword = String(PASSWORD);
String currentID = String(ID);
String currentURL = "api.thingspeak.com";
String currentAPIKey = "YF3V4U4D4C5CLVDR";

bool currentEnableConfig = false;
int currentBandwidth;
int currentCodingRate;
int currentSpreadingFactor;
int currentTxPower;
bool currentEnablePaboost;

int numNodes;

std::vector<String> nodeIDs;

AsyncWebServer server(80);

void setupAPController()
{
  if(!setupPreferences())
  {
    abort();
  }

  Serial.println("[webServer] Configuring AP Mode (Access Point)...");
  WiFi.softAP(currentID.c_str(), password);
  
  Serial.println("[webServer] Starting the server...");
  server.begin();

  Serial.print("[webServer] To configure, connect to the Wi-Fi network: ");
  Serial.println(ssid);
  Serial.print("[webServer] Open a browser and go to http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/");

  Serial.println("[webServer] Configuring routes on the web server...");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
      //HTML header modification
      String html = "<html><head><title>Gateway Configuration</title>";
      html += "<style>";
      html += "body { font-size: 140%; text-align: center; background-color: #f0f0ff; color: #800080; }";
      html += "form { text-align: left; max-width: 600px; margin: 0 auto; }";  // Aumentando a largura máxima e centralizando
      html += "label { display: block; margin-bottom: 15px; float: left; width: 40%; }";  // Aumentando o espaçamento entre os labels e os outros elementos
      html += "input[type='checkbox'] { width: auto; margin-left: 5px; float: left; clear: right; }";  // Flutuando a checkbox para a esquerda e limpando o flutuador à direita
      html += "input[type='submit'] { background-color: #800080; color: #fff; padding: 10px 15px; border: none; cursor: pointer; margin-top: 20px; }";  // Aumentando o padding do botão e adicionando margem superior
      html += "select, input[type='text'], input[type='password'], input[type='number'] { width: 60%; box-sizing: border-box; padding: 10px; margin-bottom: 20px; float: right; }";  // Aumentando o espaçamento vertical entre os elementos
      html += "select { margin-bottom: 20px; }";  // Ajustando a margem inferior das selects
      html += "div { clear: both; }";  // Limpando os flutuadores após os inputs
      html += "</style>";
      html += "</head><body>";
      html += "<h1 style='color: #800080;'>Gateway Configuration Settings</h1>";
      
      html += "<p>Wi-Fi status: " + WiFiConn::getInstance().connectionToString[WiFiConn::getInstance().connected.wifi] + ".</p>";
      
      html += "<form action='/config' method='POST'>";
      html += "<div><label for='ssid'>Network name:</label> <input type='text' name='ssid'></div>";  // Div para agrupar o label e o input
      html += "<div>Empty password? <input type='checkbox' name='noPassword'></div>";
      html += "<div><label for='password'>Network password:</label> <input type='password' name='password'></div>";
      html += "<div><label for='url'>URL API Server:</label> <input type='text' name='url'></div>";
      html += "<div><label for='APIkey'>Password Key:</label> <input type='password' name='APIKey'></div>"; 
      
      //Adding LoRa configuration options as dropdowns
      html += "<div>Configuration mode? <input type='checkbox' name='enableConfig'></div>";
      
      //Bandwidth drop-down list
      html += "<div><label for='bandwidth'>Bandwidth:</label>";
      html += "<select name='bandwidth'>";
      //html += "<option value='0'>7.8 kHz</option>";
      //html += "<option value='1'>10.4 kHz</option>";
      //html += "<option value='2'>15.6 kHz</option>";
      //html += "<option value='3'>20.8 kHz</option>";
      html += "<option value='4'>31.25 kHz</option>";
      html += "<option value='5'>41.7 kHz</option>";
      html += "<option value='6'>62.5 kHz</option>";
      html += "<option value='7'>125 kHz</option>";
      html += "<option value='8'>250 kHz</option>";
      html += "<option value='9'>500 kHz</option>";
      html += "</select></div>";
      
      //Drop-down list for coding rate
      html += "<div><label for='codingRate'>Coding Rate:</label>";
      html += "<select name='codingRate'>";
      html += "<option value='0'>5</option>";
      html += "<option value='1'>6</option>";
      html += "<option value='2'>7</option>";
      html += "<option value='3'>8</option>";
      html += "</select></div>";
      
      //Drop-down list for the spread factor
      html += "<div><label for='spreadingFactor'>Spreading Factor:</label>";
      html += "<select name='spreadingFactor'>";
      //html += "<option value='0'>SF6</option>";
      html += "<option value='1'>SF7</option>";
      html += "<option value='2'>SF8</option>";
      html += "<option value='3'>SF9</option>";
      html += "<option value='4'>SF10</option>";
      html += "<option value='5'>SF11</option>";
      html += "<option value='6'>SF12</option>";
      html += "</select></div>";
      
      //Drop-down list for transmit power (tx power)
      html += "<div><label for='txPower'>Tx Power:</label>";
      html += "<select name='txPower'>";
      html += "<option value='0'>20 dBm</option>";
      html += "<option value='1'>17 dBm</option>";
      html += "<option value='2'>14 dBm</option>";
      html += "<option value='3'>11 dBm</option>";
      html += "<option value='4'>8 dBm</option>";
      html += "<option value='5'>5 dBm</option>";
      html += "<option value='6'>2 dBm</option>";
      html += "</select></div>";
      
      //Checkbox to enable PA_BOOST
      html += "<div>Enable PA_BOOST? <input type='checkbox' name='enablePABOOST'></div>";
      
      //Centralized send button
      html += "<div style='text-align: center;'><input type='submit' value='Send'></div>";
      
      //You can add more options here as needed
      html += "</form>";
      html += "</body></html>";

      request->send(200, "text/html", html);
  });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) 
  {
      preferences.begin("configuracoes", false);
      if(request->hasParam("ssid", true) && !(request->getParam("ssid", true)->value().isEmpty())) 
      {
          currentSSID = request->getParam("ssid", true)->value();
          preferences.putString("ssid", currentSSID);
          Serial.println("[servidorWeb] New SSID: " + currentSSID);
      }
      if((request->hasParam("password", true) && !(request->getParam("password", true)->value().isEmpty())) || request->hasParam("noPassword", true)) 
      {
          currentPassword = request->getParam("password", true)->value();
          preferences.putString("password", currentPassword);
          Serial.println("[servidorWeb] New password: " + currentPassword);
      }

      //Check if the "Configuration Mode" option is selected
      currentEnableConfig = request->hasParam("enableConfig", true) && request->getParam("enableConfig", true)->value() == "on";
      
      if(request->hasParam("url", true) && currentEnableConfig) 
      {
          currentURL = request->getParam("url", true)->value();
          preferences.putString("url", currentURL);
          Serial.println("[servidorWeb] New URL: " + currentURL);
      }
      if(request->hasParam("passwordServ", true) && currentEnableConfig) 
      {
          currentAPIKey = request->getParam("APIKey", true)->value();
          preferences.putString("APIKey", currentAPIKey);
          Serial.println("[servidorWeb] New password servidor: " + currentAPIKey);
      }

      //Check if the "Configuration Mode" option is selected
      currentEnablePaboost = request->hasParam("enablePABOOST", true) && request->getParam("enablePABOOST", true)->value() == "on";

      if(currentEnableConfig)
      {
        //Go through all request parameters
        for (size_t i = 0; i < request->params(); i++) 
        {
            AsyncWebParameter* p = request->getParam(i);
            if (p->name() == "bandwidth") 
            {
                //Convert bandwidth value to int
                currentBandwidth = p->value().toInt();
            } 
            else if (p->name() == "codingRate") 
            {
                //Converts the encoding rate value to int
                currentCodingRate = p->value().toInt();
            } 
            else if (p->name() == "spreadingFactor") 
            {
                //Converts the spread factor value to int
                currentSpreadingFactor = p->value().toInt();
            } 
            else if (p->name() == "txPower") 
            {
                //Converts the transmit power value to int
                currentTxPower = p->value().toInt();
            }
            //Add more conditions as needed for other parameters
        }

        Serial.print("[servidorWeb] Current Bandwidth: "); Serial.println(currentBandwidth);
        Serial.print("[servidorWeb] Current Coding Rate: "); Serial.println(currentCodingRate);
        Serial.print("[servidorWeb] Current Spreading Factor: "); Serial.println(currentSpreadingFactor);
        Serial.print("[servidorWeb] Current Tx Power: "); Serial.println(currentTxPower);
      }

      preferences.end();
      request->redirect("/");

      Serial.println("[webServer] Restarting the wifi handler...");
      
      WiFi.disconnect();
      WiFiConn::getInstance().updateCredentials(currentSSID, currentPassword); //Wifi local

    });

    //Call the function once at the beginning to get the node IDs
    nodeIDs = scanAndCreateNodeIDs();

    numNodes = nodeIDs.size();
    while (numNodes == 0) 
    {
        Serial.println("[webServer] Looking for nodes!");

        delay(1000); //Wait a second before trying again
        nodeIDs = scanAndCreateNodeIDs();
        numNodes = nodeIDs.size(); //Attempt to rescan
    }
}

void setupAPResponder()
{
    int apCount = WiFi.scanNetworks();
    int suffix = 1;

    for (int i = 0; i < apCount; i++) 
    {
      if (WiFi.SSID(i) == currentID) 
      {
        currentID = "NODE" + String(suffix);
        suffix++;
        i = -1; //Restart the loop to check the uniqueness of the new SSID
      }
    }

    WiFi.softAP(currentID.c_str(), password);
}

std::vector<String> scanAndCreateNodeIDs() 
{
  std::vector<String> nodeIDs;

  //Scans available Wi-Fi networks
  int networkCount = WiFi.scanNetworks();
  int maxNodeNumber = -1;  //Initializes the maximum value as -1 for NODE0

  for (int i = 0; i < networkCount; i++) 
  {
    String ssid = WiFi.SSID(i);

    //Checks whether the SSID is in the format "NODE" followed by a number or not
    if (ssid.startsWith("NODE")) 
    {
      int nodeNumber;

      //Checks if the SSID is exactly "NODE"
      if (ssid == "NODE") 
      {
        nodeNumber = 0;  //Assigns 0 if it is just "NODE"
      } 
      else 
      {
        //Extracts the SSID number for other cases (ex: NODE1, NODE2, etc.)
        nodeNumber = ssid.substring(4).toInt();
      }

      //Updates the maximum value found so far
      maxNodeNumber = max(maxNodeNumber, nodeNumber);
    }
  }

  //Creates the vector of IDs based on the maximum value found
  for (int i = 0; i <= maxNodeNumber; i++) 
  {
    String nodeID = "ID" + String(i);
    nodeIDs.push_back(nodeID);
  }

  return nodeIDs;
}

bool setupPreferences()
{
  preferences.begin("configuracoes", false);
      if(!preferences.begin("configuracoes", false))
      {
        return false;
      }
      if(!preferences.getString("ssid", "").isEmpty()) 
      {
          currentSSID = preferences.getString("ssid", "");
      }
      if(!preferences.getString("password", "").isEmpty()) 
      {
          currentPassword = preferences.getString("password", "");
      }
      if(!preferences.getString("url", "").isEmpty()) 
      {
          currentURL = preferences.getString("url", "");
      }
      if(!preferences.getString("APIKey", "").isEmpty()) 
      {
          currentAPIKey = preferences.getString("APIkey", "");
      }
  preferences.end();
  return true;
}