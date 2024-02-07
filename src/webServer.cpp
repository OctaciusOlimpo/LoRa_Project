#include "webServer.h"

Preferences preferences;

const char *ssid = "GATEWAY-AP";
const char *password = "password";
const int serverPort = 80;

int nodeNumber;

#ifdef MASTER
    #ifndef ID
    #define ID "GATEWAY-AP" // Default value if not specified in build flags
    #endif
#else
    #ifndef ID
    #define ID "NODE" // Default value if not specified in build flags
    #endif
#endif

#ifndef WIFISSID
#define WIFISSID "Rorschach" // Default value if not specified in build flags
#endif
#ifndef PASSWORD
#define PASSWORD "espectral" // Default value if not specified in build flags
#endif

String currentSSID = String(WIFISSID);
String currentPassword = String(PASSWORD);
String currentID = String(ID);
String currentURL = "api.thingspeak.com";
String currentAPIKey = "YF3V4U4D4C5CLVDR";

int numNodes;

std::vector<String> nodeIDs;

AsyncWebServer server (80);
WiFiClient client;

void setupWebServer()
{
    #ifdef MASTER
        // Configuração inicial do Master
        setupAPMaster();
    #else
        // Configuração inicial do Slave
        setupAPSlave();
    #endif
    
}

void setupAPMaster()
{
    Serial.println("[webServer] Configuring AP Mode (Access Point)...");
    WiFi.softAP(currentID.c_str(), password);

    /*
    preferences.begin("configuracoes", false);

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
    */
    
    Serial.println("[webServer] Starting the server...");
    server.begin();

    Serial.print("[webServer] To configure, connect to the Wi-Fi network: ");
    Serial.println(ssid);
    Serial.print("[webServer] Open a browser and go to http://");
    Serial.print(WiFi.softAPIP());
    Serial.println("/");
    
    // internetModule* internetController = &internetModule::getSingleton();

    Serial.println("[webServer] Configuring routes on the web server...");
    server.on("/", HTTP_GET, [/*internetController*/](AsyncWebServerRequest *request) 
    {
        // Modificação do cabeçalho HTML
        String html = "<html><head><title>Gateway Configuration</title>";
        html += "<style>";
        html += "body { font-size: 140%; text-align: center; background-color: #f0f0ff; color: #800080; }";
        html += "form { text-align: left; max-width: 400px; margin: 0 auto; }";
        html += "label { display: block; margin-bottom: 10px; }";
        html += "input { width: 100%; box-sizing: border-box; padding: 5px; margin-bottom: 15px; }";
        html += "input[type='checkbox'] { width: auto; margin-left: 5px; }";
        html += "input[type='submit'] { background-color: #800080; color: #fff; padding: 8px 15px; border: none; cursor: pointer; }";
        html += "</style>";
        html += "</head><body>";
        html += "<h1 style='color: #800080;'>Gateway Configuration Settings</h1>";
        // html += "<p>Wi-Fi status: " + internetController->connectionToString[internetController->connected.wifi] + ".</p>";
        html += "<form action='/config' method='POST'>";
        html += "<label for='ssid'>Network name:</label> <input type='text' name='ssid'><br>";
        html += "Empty password? <input type='checkbox' name='noPassword'><br>";
        html += "<label for='password'>Network password:</label> <input type='password' name='password'><br>";
        html += "<label for='url'>URL API Server:</label> <input type='text' name='url'><br>";
        html += "<label for='APIkey'>Password Key:</label> <input type='password' name='APIKey'>"; 
        html += "<input type='submit' value='Send'>";
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
            Serial.println("[servidorWeb] Nova SSID: " + currentSSID);
        }
        if((request->hasParam("password", true) && !(request->getParam("password", true)->value().isEmpty())) || (request->hasParam("noPassword", true) == true)) 
        {
            currentPassword = request->getParam("password", true)->value();
            preferences.putString("password", currentPassword);
            Serial.println("[servidorWeb] Nova password: " + currentPassword);
        }
        if(request->hasParam("url", true)) 
        {
            currentURL = request->getParam("url", true)->value();
            preferences.putString("url", currentURL);
            Serial.println("[servidorWeb] Novo URL: " + currentURL);
        }
        if(request->hasParam("passwordServ", true)) 
        {
            currentAPIKey = request->getParam("APIKey", true)->value();
            preferences.putString("APIKey", currentAPIKey);
            Serial.println("[servidorWeb] Nova senha servidor: " + currentAPIKey);
        }

        preferences.end();
        request->redirect("/");

        // Desconecte completamente o wifi
        Serial.println("[webServer] Restarting the wifi handler...");
        WiFi.disconnect();
    });

    // Chama a função uma vez no início para obter os IDs dos nodes
    nodeIDs = scanAndCreateNodeIDs();

    numNodes = nodeIDs.size();
    while (numNodes == 0) 
    {
        Serial.println("[webServer] Looking for nodes!");

        delay(1000); // Espera um segundo antes de tentar novamente
        nodeIDs = scanAndCreateNodeIDs();
        numNodes = nodeIDs.size(); // Tentativa de reescanear
    }
}

void setupAPSlave()
{
    int apCount = WiFi.scanNetworks();
    int suffix = 1;

    for (int i = 0; i < apCount; i++) 
    {
      if (WiFi.SSID(i) == currentID) 
      {
        currentID = "NODE" + String(suffix);
        suffix++;
        i = -1; // Reiniciar o loop para verificar a singularidade do novo SSID
      }
    }

    WiFi.softAP(currentID.c_str(), password);
}

std::vector<String> scanAndCreateNodeIDs() 
{
  std::vector<String> nodeIDs;

  // Realiza a varredura das redes Wi-Fi disponíveis
  int networkCount = WiFi.scanNetworks();
  int maxNodeNumber = -1;  // Inicializa o valor máximo como -1 para NODE0

  for (int i = 0; i < networkCount; i++) 
  {
    String ssid = WiFi.SSID(i);

    // Verifica se o SSID está no formato "NODE" seguido ou não por um número
    if (ssid.startsWith("NODE")) 
    {
      int nodeNumber;

      // Verifica se o SSID é exatamente "NODE"
      if (ssid == "NODE") 
      {
        nodeNumber = 0;  // Atribui 0 se for apenas "NODE"
      } 
      else 
      {
        // Extrai o número do SSID para outros casos (ex: NODE1, NODE2, etc.)
        nodeNumber = ssid.substring(4).toInt();
      }

      // Atualiza o valor máximo encontrado até agora
      maxNodeNumber = max(maxNodeNumber, nodeNumber);
    }
  }

  // Cria o vetor de IDs com base no valor máximo encontrado
  for (int i = 0; i <= maxNodeNumber; i++) 
  {
    String nodeID = "ID" + String(i);
    nodeIDs.push_back(nodeID);
  }

  return nodeIDs;
}