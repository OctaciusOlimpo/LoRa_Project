import random
import json
import datetime as dt
from paho.mqtt import client as mqtt_client
import influxdb_client
from influxdb_client import Point
from influxdb_client.client.write_api import SYNCHRONOUS

BROKER_HOST = 'mosquitto'
BROKER_PORT = 1883
TOPIC = "teste"
BROKER_USERNAME = 'esp32-gateway-1'
BROKER_PASSWORD = 'wonder'
CLIENT_ID = f'python-mqtt-{random.randint(0, 1000)}'

INFLUX_HOST = 'http://influx:8086'
INFLUX_ORG = 'lora'
INFLUX_BUCKET = 'lora'
INFLUX_TOKEN = '383502ea-9354-49f6-81b5-ce9c072e9676'

db = influxdb_client.InfluxDBClient(org=INFLUX_ORG,
                                    url=INFLUX_HOST,
                                    token=INFLUX_TOKEN)
db_write = db.write_api(write_options=SYNCHRONOUS)

def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC)
    print("Conectado ao broker MQTT")

def on_disconnect(client, userdata, flags):
    print("Desconectado do broker MQTT")

def on_message(client, userdata, msg):
    try:
        # Decodificar a mensagem JSON
        payload = json.loads(msg.payload)

        # Verificar o tipo de dados da temperatura
        if isinstance(payload["temperature"], int):
            # Se for um número inteiro, converter para float
            payload["temperature"] = float(payload["temperature"])

        # Verificar o tipo de dados da umidade
        if isinstance(payload["humidity"], int):
            # Se for um número inteiro, converter para float
            payload["humidity"] = float(payload["humidity"])

        # Verificar o tipo de dados do rssi
        if isinstance(payload["rssi"], int):
            # Se for um número inteiro, converter para float
            payload["rssi"] = float(payload["rssi"])

        # Criar um ponto para escrever no banco de dados
        point = Point("sensor") \
            .tag("id", payload["id"]) \
            .field("temperature", payload["temperature"]) \
            .field("humidity", payload["humidity"]) \
            .field("rssi", payload["rssi"])

        # Gravar o ponto no banco de dados
        db_write.write(bucket=INFLUX_BUCKET, record=point)

        # Imprimir os dados incluindo o rssi
        print("Dados gravados com sucesso:", point.to_line_protocol())

    except Exception as e:
        print("Erro ao processar mensagem:", e)

def run():
    broker = mqtt_client.Client(CLIENT_ID)
    broker.username_pw_set(BROKER_USERNAME, BROKER_PASSWORD)
    broker.on_connect = on_connect
    broker.on_disconnect = on_disconnect
    broker.on_message = on_message

    try:
        broker.connect(BROKER_HOST, BROKER_PORT)
        print("Conectado ao broker MQTT")
    except ConnectionRefusedError as cre:
        print("Erro de conexão:", cre)
    except Exception as e:
        print("Erro ao conectar:", e)

    broker.loop_forever()

if __name__ == '__main__':
    run()
