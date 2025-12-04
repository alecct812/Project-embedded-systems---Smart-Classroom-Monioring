# 🔍 Diagnóstico de Conexão MQTT

## ❌ Erro Atual: `-2` (MQTT_CONNECT_FAILED)

O erro `-2` significa que o ESP32 **não consegue alcançar o servidor MQTT**.

## ✅ Checklist de Diagnóstico

### 1️⃣ Verificar se o Broker MQTT está rodando

**Se você usa Node-RED:**
```bash
# Verifique se o Node-RED está rodando
# Acesse: http://172.26.70.17:1880
```

**Se você usa Mosquitto:**
```bash
# Verifique se o Mosquitto está rodando
ps aux | grep mosquitto

# Ou teste a conexão
mosquitto_sub -h 172.26.70.17 -p 1883 -t "teste"
```

### 2️⃣ Verificar se o IP está correto

O código está configurado para: `172.26.70.17:1883`

**Confirme qual é o IP real do seu broker MQTT:**
- Se for Node-RED, qual IP está configurado?
- Se for Mosquitto, qual IP da máquina onde está rodando?

### 3️⃣ Verificar se ESP32 e Broker estão na mesma rede

O ESP32 precisa estar na **mesma rede Wi-Fi** que o broker.

**Verifique o IP do ESP32:**
- No monitor serial, você deve ver: `IP: 192.168.x.x` ou `172.26.x.x`
- Se o IP começar com `192.168`, mas o broker está em `172.26`, estão em redes diferentes!

### 4️⃣ Testar conectividade de rede

**Do ESP32 para o Broker:**
- Infelizmente não podemos fazer ping direto do ESP32
- Mas podemos verificar se o Wi-Fi conectou corretamente

**Do seu computador para o Broker:**
```bash
# Teste se consegue alcançar o broker
ping 172.26.70.17

# Teste a porta MQTT
telnet 172.26.70.17 1883
# Ou
nc -zv 172.26.70.17 1883
```

### 5️⃣ Verificar Firewall

O firewall pode estar bloqueando a porta 1883.

**Se o broker está em outra máquina:**
- Verifique se a porta 1883 está aberta
- No Linux: `sudo ufw allow 1883`
- No Windows: Configure o firewall para permitir porta 1883

## 🔧 Soluções Possíveis

### Solução 1: Corrigir o IP do Broker

Se o IP do broker for diferente de `172.26.70.17`, edite `main.cpp`:

```cpp
const char *mqtt_server = "SEU_IP_AQUI";  // Ex: "192.168.1.100"
```

### Solução 2: Usar Broker Público para Teste

Para testar se o problema é o broker, use um broker público temporariamente:

```cpp
const char *mqtt_server = "broker.emqx.io";  // Broker público
```

### Solução 3: Verificar se Broker Aceita Conexões Externas

Alguns brokers só aceitam conexões de `localhost`. Configure para aceitar conexões de qualquer IP:

**Mosquitto (`mosquitto.conf`):**
```
listener 1883 0.0.0.0
allow_anonymous true
```

**Node-RED:** Verifique se o broker MQTT está configurado para aceitar conexões externas.

### Solução 4: Usar IP da Rede Local

Se o ESP32 está em rede `192.168.x.x`, o broker também precisa estar acessível nessa rede:

```cpp
const char *mqtt_server = "192.168.1.XXX";  // IP local do broker
```

## 📝 Próximos Passos

1. ✅ **Confirme qual é o IP correto do seu broker MQTT**
2. ✅ **Verifique se o broker está rodando**
3. ✅ **Atualize o código com o IP correto**
4. ✅ **Faça upload novamente**
5. ✅ **Teste a conexão**

## 🆘 Informações que Preciso

Para ajudar melhor, me diga:

1. **Qual IP está usando para o broker MQTT?**
2. **O broker está rodando? (Node-RED, Mosquitto, outro?)**
3. **Qual IP o ESP32 recebeu quando conectou ao Wi-Fi?** (aparece no monitor serial)
4. **O broker está na mesma rede que o ESP32?**

---

**💡 Dica:** O erro `-2` quase sempre é causado por IP incorreto ou broker não acessível na rede.

