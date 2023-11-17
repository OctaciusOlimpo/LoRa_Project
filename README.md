# Projeto de Desenvolvimento de Rede IoT com LoRa

<p align="center">
  <img width="200" src="images/LoRa_Logo.png" alt="LoRa Logo">
</p>

Este repositório é dedicado ao desenvolvimento de uma rede IoT para sensoriamento, utilizando comunicação por radiofrequência LoRa (Long Range). O projeto envolve a implementação de um esquema estrela, no qual os elementos "gateway" e os "end-devices" colaboram para coletar dados de sensores e encaminhá-los para a plataforma Thingspeak.

## Arquitetura da Rede

A rede é composta por dois tipos principais de elementos:

1. **Gateway:**
   - Baseado na placa de desenvolvimento Heltec Esp32 LoRa.
   - Responsável por receber dados dos end-devices e encaminhá-los para a plataforma Thingspeak.

2. **End-devices:**
   - Baseados na placa de desenvolvimento Heltec Esp32 LoRa.
   - Equipados com o sensor de temperatura e umidade DHT-11.
   - Envia informações de sensoriamento para o gateway.

<p align="center">
  <img width="200" src="images/HeltecEsp32LoRa.png" alt="LoRa Logo">
</p>

## Funcionalidades Principais

- **Sensoriamento:**
  - Os end-devices utilizam o sensor DHT-11 para coletar dados de temperatura e umidade.
  - Esses dados são enviados para o gateway.

- **Comunicação LoRa:**
  - A comunicação entre os end-devices e o gateway é baseada na tecnologia LoRa, proporcionando uma longa distância de alcance.

- **Esquema Estrela:**
  - O esquema de comunicação é organizado em uma topologia estrela, onde os end-devices enviam informações diretamente para o gateway.

- **Integração com Thingspeak:**
  - O gateway encaminha os dados de sensoriamento para a plataforma Thingspeak, permitindo monitoramento e análise remotos.

## Estrutura do Repositório

- `/docs`: Documentação do projeto.
- `/src`: Código-fonte do firmware para os end-devices e o gateway.
- `/schematics`: Esquemáticos e diagramas de circuito.

## Configuração e Instalação

1. Clone este repositório:
   ```bash
   git clone https://github.com/kevincsouto/LoRa_Project.git

2. Siga as instruções específicas para configurar e instalar o firmware nos end-devices e no gateway no diretório /src.
3. Consulte a documentação no diretório /docs para obter informações detalhadas sobre a configuração e operação da rede.

## Contribuição
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues, propor melhorias ou enviar pull requests.

## Licença
Este projeto é licenciado sob a Licença MIT. Veja o arquivo LICENSE para mais detalhes.
