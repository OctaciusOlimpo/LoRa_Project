#include "webserver.h"

Preferences preferences;

const char *ssid = "GATEWAY-AP";
const char *password = "password";
const int serverPort = 80;

int nodeNumber;

#ifdef CONTROLLER
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
        // Modificação do cabeçalho HTML
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
        html += "<form action='/config' method='POST'>";
        html += "<div><label for='ssid'>Network name:</label> <input type='text' name='ssid'></div>";  // Div para agrupar o label e o input
        html += "<div>Empty password? <input type='checkbox' name='noPassword'></div>";
        html += "<div><label for='password'>Network password:</label> <input type='password' name='password'></div>";
        html += "<div><label for='url'>URL API Server:</label> <input type='text' name='url'></div>";
        html += "<div><label for='APIkey'>Password Key:</label> <input type='password' name='APIKey'></div>"; 
        
        // Adicionando opções de configuração LoRa como listas suspensas
        html += "<div>Configuration mode? <input type='checkbox' name='enableConfig'></div>";
        
        // Lista suspensa para a largura de banda (bandwidth)
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
        
        // Lista suspensa para a taxa de codificação (coding rate)
        html += "<div><label for='codingRate'>Coding Rate:</label>";
        html += "<select name='codingRate'>";
        html += "<option value='0'>5</option>";
        html += "<option value='1'>6</option>";
        html += "<option value='2'>7</option>";
        html += "<option value='3'>8</option>";
        html += "</select></div>";
        
        // Lista suspensa para o fator de espalhamento (spreading factor)
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
        
        // Lista suspensa para a potência de transmissão (tx power)
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
        
        // Checkbox para habilitar PA_BOOST
        html += "<div>Enable PA_BOOST? <input type='checkbox' name='enablePABOOST'></div>";
        
        // Botão de envio centralizado
        html += "<div style='text-align: center;'><input type='submit' value='Send'></div>";
        
        // Você pode adicionar mais opções aqui conforme necessário
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

        // Verifica se a opção "Configuration Mode" está selecionada
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

        // Verifica se a opção "Configuration Mode" está selecionada
        currentEnablePaboost = request->hasParam("enablePABOOST", true) && request->getParam("enablePABOOST", true)->value() == "on";

        if(currentEnableConfig)
        {
          // Percorre todos os parâmetros da requisição
          for (size_t i = 0; i < request->params(); i++) 
          {
              AsyncWebParameter* p = request->getParam(i);
              if (p->name() == "bandwidth") 
              {
                  // Converte o valor da largura de banda para int
                  currentBandwidth = p->value().toInt();
              } 
              else if (p->name() == "codingRate") 
              {
                  // Converte o valor da taxa de codificação para int
                  currentCodingRate = p->value().toInt();
              } 
              else if (p->name() == "spreadingFactor") 
              {
                  // Converte o valor do fator de espalhamento para int
                  currentSpreadingFactor = p->value().toInt();
              } 
              else if (p->name() == "txPower") 
              {
                  // Converte o valor da potência de transmissão para int
                  currentTxPower = p->value().toInt();
              }
              // Adicione mais condições conforme necessário para outros parâmetros
          }

          Serial.print("[servidorWeb] Current Bandwidth: "); Serial.println(currentBandwidth);
          Serial.print("[servidorWeb] Current Coding Rate: "); Serial.println(currentCodingRate);
          Serial.print("[servidorWeb] Current Spreading Factor: "); Serial.println(currentSpreadingFactor);
          Serial.print("[servidorWeb] Current Tx Power: "); Serial.println(currentTxPower);
        }

        preferences.end();
        request->redirect("/");

        // Desconecte completamente o wifi
        Serial.println("[webServer] Restarting the wifi handler...");
        WiFi.disconnect();
        wifiConn->connect();
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