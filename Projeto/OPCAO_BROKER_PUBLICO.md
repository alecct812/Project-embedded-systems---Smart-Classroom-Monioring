# 🌐 Opção Simples: Usar Broker MQTT Público

Se você não quer instalar Mosquitto ou Aedes, pode usar um **broker MQTT público gratuito**!

## ✅ Vantagens
- ✨ Não precisa instalar nada extra
- ✨ Funciona imediatamente
- ✨ Configuração super simples
- ✨ Ideal para testes e demonstrações

## ⚠️ Desvantagens
- Qualquer pessoa pode ver seus dados (tópicos públicos)
- Menos confiável que broker local
- Depende de internet

---

## 🚀 Configuração Rápida

### **1. Alterar o Código do ESP32**

Edite `src/main.cpp` e altere estas linhas:

```cpp
// ANTES (broker local)
const char *mqtt_server = "192.168.1.100";
const int mqtt_port = 1883;

// DEPOIS (broker público HiveMQ)
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
```

### **2. Configurar Node-RED**

#### **Opção A: Editar manualmente cada nó MQTT**
1. No Node-RED, clique duas vezes em cada nó **"mqtt in"**
2. Clique no ícone de lápis ao lado de "Server"
3. Altere:
   - **Server**: `broker.hivemq.com`
   - **Port**: `1883`
   - **Client ID**: deixe vazio (gera automaticamente)
4. Clique em **Update** e depois **Done**
5. Repita para TODOS os nós mqtt in
6. Clique em **Deploy**

#### **Opção B: Usar o flow já configurado**

Copie e cole este flow no Node-RED (Menu → Import):

```json
[
    {
        "id": "mqtt_temp",
        "type": "mqtt in",
        "name": "Temperatura",
        "topic": "sala/temperatura",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 100,
        "wires": [["debug_temp"]]
    },
    {
        "id": "debug_temp",
        "type": "debug",
        "name": "📊 Temperatura",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 100,
        "wires": []
    },
    {
        "id": "mqtt_umid",
        "type": "mqtt in",
        "name": "Umidade",
        "topic": "sala/umidade",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 180,
        "wires": [["debug_umid"]]
    },
    {
        "id": "debug_umid",
        "type": "debug",
        "name": "💧 Umidade",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 180,
        "wires": []
    },
    {
        "id": "mqtt_luz",
        "type": "mqtt in",
        "name": "Luminosidade",
        "topic": "sala/luminosidade",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 260,
        "wires": [["debug_luz"]]
    },
    {
        "id": "debug_luz",
        "type": "debug",
        "name": "💡 Luminosidade",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 260,
        "wires": []
    },
    {
        "id": "mqtt_presenca",
        "type": "mqtt in",
        "name": "Presença",
        "topic": "sala/presenca",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 340,
        "wires": [["debug_presenca"]]
    },
    {
        "id": "debug_presenca",
        "type": "debug",
        "name": "👤 Presença",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 340,
        "wires": []
    },
    {
        "id": "mqtt_ocupacao",
        "type": "mqtt in",
        "name": "Ocupação",
        "topic": "sala/ocupacao",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 420,
        "wires": [["debug_ocupacao"]]
    },
    {
        "id": "debug_ocupacao",
        "type": "debug",
        "name": "🚪 Ocupação",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 420,
        "wires": []
    },
    {
        "id": "mqtt_alertas",
        "type": "mqtt in",
        "name": "Alertas",
        "topic": "sala/alertas",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 500,
        "wires": [["debug_alertas"]]
    },
    {
        "id": "debug_alertas",
        "type": "debug",
        "name": "⚠️ Alertas",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 500,
        "wires": []
    },
    {
        "id": "mqtt_ac",
        "type": "mqtt in",
        "name": "Sugestão AC",
        "topic": "sala/sugestao_ac",
        "qos": "0",
        "datatype": "auto",
        "broker": "broker_hivemq",
        "x": 150,
        "y": 580,
        "wires": [["debug_ac"]]
    },
    {
        "id": "debug_ac",
        "type": "debug",
        "name": "❄️ Sugestão AC",
        "active": true,
        "tosidebar": true,
        "console": false,
        "complete": "payload",
        "x": 400,
        "y": 580,
        "wires": []
    },
    {
        "id": "broker_hivemq",
        "type": "mqtt-broker",
        "name": "HiveMQ Public",
        "broker": "broker.hivemq.com",
        "port": "1883",
        "clientid": "",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": "4",
        "keepalive": "60",
        "cleansession": true
    }
]
```

---

## 📊 Outros Brokers Públicos Gratuitos

Você pode usar qualquer um destes (todos funcionam igual):

| Broker | Servidor | Porta |
|--------|----------|-------|
| **HiveMQ** | `broker.hivemq.com` | `1883` |
| **Eclipse** | `mqtt.eclipseprojects.io` | `1883` |
| **Mosquitto Test** | `test.mosquitto.org` | `1883` |
| **EMQX** | `broker.emqx.io` | `1883` |

---

## 🧪 Testar se Funciona

### **1. No ESP32 Serial Monitor:**
Deve aparecer:
```
Conectando ao MQTT...Conectado!
📊 Temperatura: 23.50 °C
💧 Umidade: 55.20 %
```

### **2. No Node-RED Debug:**
Deve aparecer as mensagens:
```
sala/temperatura: "23.50"
sala/umidade: "55.20"
sala/luminosidade: "75.00"
```

---

## 🔒 Segurança (Opcional)

Para evitar conflito com outros usuários do broker público, use **tópicos únicos**:

No código ESP32, altere:
```cpp
// Adicione seu nome/grupo aos tópicos
client.publish("grupo10/sala/temperatura", buffer);
client.publish("grupo10/sala/umidade", buffer);
// etc...
```

No Node-RED, altere os tópicos:
```
grupo10/sala/temperatura
grupo10/sala/umidade
grupo10/sala/luminosidade
// etc...
```

---

## ✅ Pronto!

Agora seu sistema funciona **sem precisar instalar nenhum broker local**! 🎉

Ideal para:
- ✅ Testes rápidos
- ✅ Demonstrações
- ✅ Desenvolvimento
- ✅ Apresentação do trabalho

**Para produção real**, é recomendado usar um broker local (Mosquitto ou Aedes) para maior segurança e confiabilidade.
