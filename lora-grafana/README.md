# Visualização LoRa Grafana

## Executando o projeto

### Configurando o broker

Primeiro, deve-se expor o broker de alguma forma. O `ngrok` permite que faça
isso através de um IP público:

```shell
$ ngrok tcp <port>
```

Se estiver executando o broker na mesma máquina, utilize o _host_ como
`localhost`, e a porta como a que estiver configurada localmente.

Atualize as seguintes variáveis no arquivo [client.py](./client.py):

* `BROKER_HOST`: _host_ do broker;
* `BROKER_PORT`: _port_ do broker;
* `BROKER_USERNAME`: usuário para autenticação no broker;
* `BROKER_PASSWORD`: senha para autenticação no broker;
* `TOPIC`: tópico que será consumido do broker.

### Configurações do banco de dados

Não é preciso alterar nada relacionado ao banco de dados. As variáveis
importantes no script [client.py](./client.py) são:

* `INFLUX_HOST`: _host_ do banco de dados, incluindo protocolo (`http`) e
    _port_, e.g., `http://localhost:8086`;
* `INFLUX_ORG`: organização no banco de dados que será utilizada;
* `INFLUX_BUCKET`: _bucket_ no banco de dados utilizado para armazenar os dados;
* `INFLUX_TOKEN`: _token_ para autenticação no banco de dados.

As variáveis devem ser configuradas de acordo com os valores presentes no
[`docker-compose.yml`](./docker-compose.yml).

### Executando o projeto

Crie um ambiente virtual do `python`, ative-o e instale as dependências:

```shell
python -m venv venv
./venv/bin/activate
pip install -r requirements.txt
```

Execute o `docker-compose` e espere que todos os containers sejam configurados:

```shell
docker-compose up
```

Execute o projeto e observe os logs para certificar-se de que foi possível
conectar-se ao broker e que mensagens estão sendo publicadas (Elas aparecerão
como _logs_.):

```
python client.py
```

Acesse o Grafana no browser pelo URL [http://localhost:3000/](http://localhost:3000/)
e preencha as credenciais. (Usuário: `admin`; senha: `admin`.) Se quiser,
configure um nova senha.

Configure o InfluxDB como fonte de dados:

* No canto superior direito, clique no `+`;
* Selecione a opção `New dashboard`;
* Clique no botão `+ Add visualization`;
* Clique no botão `Configure a new data source`;
* Selecione `InfluxDB`;
* Configurações do _data source_:
    * `Query Language`: `Flux`;
    * `URL`: `http://influx:8086`;
    * `InfluxDB Details`:
        * `Organization`: `lora`;
        * `Token`: O valor registrado no
            [docker-compose.yml](./docker-compose.yml);
* Clique no botão `Save & test`;
* Se uma mensagem de sucesso aparecer, clique no botão `Explore data` e comece a
    fazer queries no banco de dados.
