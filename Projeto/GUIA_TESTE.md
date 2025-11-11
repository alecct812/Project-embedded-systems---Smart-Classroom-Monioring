# 🚀 Guia Rápido de Teste - Sistema de Sala Inteligente

## ✅ Checklist Pré-Teste

### 1. Hardware
- [ ] ESP32 conectado via USB
- [ ] DHT11 conectado no GPIO 15
- [ ] LDR conectado no GPIO 2
- [ ] PIR conectado no GPIO 13
- [ ] LED conectado no GPIO 32 (com resistor 220Ω)
- [ ] Todos os GNDs conectados
- [ ] Alimentação dos sensores (3.3V ou 5V conforme sensor)

### 2. Software
- [ ] VS Code + PlatformIO instalado
- [ ] Node-RED instalado e rodando
- [ ] node-red-contrib-aedes (Broker MQTT integrado ao Node-RED)
- [ ] Flow importado no Node-RED

---

## 📝 Passo a Passo para Testar

### **1. Configurar Wi-Fi e MQTT**

Edite `src/main.cpp` (linhas 39-42):

```cpp
const char *ssid = "SEU_WIFI_AQUI";           // ALTERAR
const char *password = "SUA_SENHA_AQUI";      // ALTERAR
const char *mqtt_server = "192.168.1.XXX";    // ALTERAR (IP do seu PC)
```

**Como descobrir o IP do seu PC:**
```powershell
# Windows PowerShell
ipconfig
# Procure por "Endereço IPv4" na sua rede Wi-Fi
```

**Importante:** Como você está usando Node-RED como broker, use o IP do PC onde o Node-RED está rodando!

---

### **2. Instalar Broker MQTT no Node-RED**

O Node-RED precisa de um broker MQTT. Vamos instalar o **Aedes** (broker MQTT que roda dentro do Node-RED):

```bash
# Parar o Node-RED se estiver rodando (Ctrl+C)
cd ~/.node-red
npm install node-red-contrib-aedes
```

Depois reinicie o Node-RED:
```bash
node-red
```

---

### **3. Iniciar Node-RED**

```bash
node-red
```

Acesse: `http://localhost:1880`

---

### **4. Configurar Broker MQTT no Node-RED**

1. Após importar o flow (próximo passo), você verá um nó **"Aedes Broker"**
2. Este nó cria um broker MQTT na porta **1883**
3. O ESP32 se conectará a este broker

**Ou, se preferir usar broker externo online:**
- Pode usar o **broker.hivemq.com** (público, gratuito)
- Altere no código e nos nós MQTT do Node-RED

---

### **5. Importar Flow no Node-RED**

1. Abra `http://localhost:1880`
2. Clique em **Menu (☰) → Import**
3. Clique em **select a file to import**
4. Selecione o arquivo `node-red-flow.json`
5. Clique em **Import**
6. Clique em **Deploy** (botão vermelho no canto superior direito)

---

### **6. Fazer Upload do Código para o ESP32**

#### No VS Code:
1. Abra o projeto no VS Code
2. Conecte o ESP32 via USB
3. **Ctrl + Alt + U** (ou clique no ícone ➡️ "Upload")
4. Aguarde a compilação e upload (pode levar 1-2 minutos)

---

### **7. Abrir Monitor Serial**

#### No VS Code:
- **Ctrl + Alt + S** (ou clique no ícone 🔌 "Serial Monitor")
- Baud rate deve estar em **115200**

#### Você deve ver:
```
╔════════════════════════════════════════════════╗
║  Sistema de Monitoramento de Sala Inteligente ║
║              GRUPO 10 - ESP32                  ║
╚════════════════════════════════════════════════╝

Conectando ao Wi-Fi: uaifai-apolo
.....
WiFi conectado!
Endereço IP: 192.168.1.105

Conectando ao MQTT...Conectado!

✓ DHT11 detectado no GPIO 15
✓ LDR detectado no GPIO 2
✓ PIR configurado no GPIO 13
✓ LED configurado no GPIO 32

✓ Task DHT11 criada no Core 0
✓ Task LDR criada no Core 0
✓ Task PIR criada no Core 0
✓ Task MQTT criada no Core 1 (prioridade ALTA)

🚀 Sistema iniciado com sucesso!
📡 Monitorando sensores e publicando via MQTT...

📊 Temperatura: 23.50 °C
💧 Umidade: 55.20 %
💡 Luminosidade: 75.00 %
```

---

### **8. Visualizar Dados no Node-RED**

#### Debug Console:
1. No Node-RED, clique na aba **🐛 Debug** (lado direito)
2. Você deve ver as mensagens MQTT chegando:
   ```
   sala/temperatura: "23.50"
   sala/umidade: "55.20"
   sala/luminosidade: "75.00"
   sala/presenca: "NAO_DETECTADA"
   sala/ocupacao: "VAZIA"
   ```

#### Dashboard (se instalou node-red-dashboard):
- Acesse: `http://localhost:1880/ui`
- Veja os gráficos e indicadores em tempo real

---

## 🧪 Testes para Realizar

### **Teste 1: Sensor de Temperatura/Umidade (DHT11)**
- ✅ Aproxime a mão do sensor (deve aumentar temperatura)
- ✅ Assopre o sensor (deve aumentar umidade)
- ✅ Verifique se os valores aparecem no Serial e Node-RED

### **Teste 2: Sensor de Luminosidade (LDR)**
- ✅ Cubra o LDR com a mão (luminosidade deve cair)
- ✅ Aponte uma lanterna (luminosidade deve subir)
- ✅ Verifique valores entre 0% e 100%

### **Teste 3: Sensor de Presença (PIR)**
- ✅ Aguarde 60 segundos para o PIR calibrar (após ligar)
- ✅ Movimente a mão na frente do sensor
- ✅ **O LED deve ACENDER** quando detectar movimento
- ✅ No Serial deve aparecer: `👤 PRESENÇA DETECTADA - Sala OCUPADA - LED LIGADO`
- ✅ No Node-RED: `sala/presenca: "DETECTADA"` e `sala/ocupacao: "OCUPADA"`
- ✅ Fique parado por 5-10 segundos
- ✅ **O LED deve APAGAR**
- ✅ No Serial: `🚪 Presença NÃO detectada - Sala VAZIA - LED DESLIGADO`

### **Teste 4: Alertas de Temperatura**
- ✅ Aumente a temperatura acima de 25°C (assopre ar quente)
- ✅ Deve aparecer: `⚠️ ALERTA: Temperatura fora do ideal`
- ✅ Deve sugerir: `❄️ Sugestão: LIGAR ar-condicionado`

### **Teste 5: Alertas de Umidade**
- ✅ Se umidade < 40% ou > 60%
- ✅ Deve aparecer: `⚠️ ALERTA: Umidade fora do ideal`

---

## 🐛 Troubleshooting

### ❌ ESP32 não conecta ao Wi-Fi
**Sintoma:** Monitor Serial fica em `Conectando ao Wi-Fi: ...........`

**Solução:**
1. Verifique se o SSID e senha estão corretos
2. Confirme que é uma rede **2.4 GHz** (ESP32 não suporta 5 GHz)
3. Aproxime o ESP32 do roteador

---

### ❌ Não conecta ao MQTT
**Sintoma:** `Falha, rc=-2 - Tentando novamente em 5s`

**Solução:**
1. Verifique se o Node-RED está rodando
2. Verifique se o nó **Aedes Broker** está no flow e ativo
3. Confirme o IP do broker no código (deve ser o IP do PC onde roda o Node-RED)
4. Alternativa: Use broker público temporariamente:
   ```cpp
   const char *mqtt_server = "broker.hivemq.com"; // Broker público
   ```
   E altere também nos nós MQTT do Node-RED para `broker.hivemq.com`

---

### ❌ Sensores não detectados
**Sintoma:** `✗ DHT11 NÃO detectado` ou `✗ LDR NÃO detectado`

**Solução DHT11:**
1. Verifique as conexões:
   - VCC → 3.3V
   - GND → GND
   - DATA → GPIO 15
2. DHT11 precisa de 2 segundos para inicializar
3. Reinicie o ESP32

**Solução LDR:**
1. Verifique o divisor de tensão:
   ```
   3.3V ─── LDR ─── GPIO 2 ─── Resistor 10kΩ ─── GND
   ```
2. O LDR pode estar invertido

**Solução PIR:**
1. Aguarde 60 segundos após ligar (calibração)
2. Verifique:
   - VCC → 5V
   - GND → GND
   - OUT → GPIO 13

---

### ❌ Node-RED não recebe dados
**Sintoma:** Debug vazio, nenhuma mensagem

**Solução:**
1. Verifique se o flow está "Deployed" (botão vermelho)
2. Verifique se o nó **"Aedes Broker"** está presente e ativo (sem erros)
3. Clique nos nós "mqtt in" e verifique:
   - Server: deve apontar para o broker configurado
   - Topic: deve estar correto (ex: `sala/temperatura`)
4. Certifique-se que o ESP32 está conectado (veja Serial Monitor mostrando publicações)
5. No Node-RED, adicione um nó **"mqtt in"** com tópico `#` (todos os tópicos) para debug

---

### ❌ LED não acende
**Sintoma:** PIR detecta (Serial mostra), mas LED não liga

**Solução:**
1. Verifique:
   - LED ânodo (+) → GPIO 32
   - LED cátodo (-) → Resistor 220Ω → GND
2. Teste o LED diretamente:
   ```cpp
   digitalWrite(LED_PIN, HIGH);
   delay(2000);
   digitalWrite(LED_PIN, LOW);
   ```

---

## 📊 Valores Esperados

### Ambiente Normal (sala de aula):
- **Temperatura:** 20°C - 26°C
- **Umidade:** 40% - 70%
- **Luminosidade:** 30% - 80% (com luz acesa)
- **Luminosidade:** 5% - 20% (sala escura)

### Alertas devem disparar quando:
- Temperatura < 20°C ou > 25°C
- Umidade < 40% ou > 60%

---

## 📸 Evidências para o Trabalho

Capture:
1. ✅ Monitor Serial mostrando detecção de sensores
2. ✅ Monitor Serial mostrando dados sendo publicados
3. ✅ Node-RED Debug Console com mensagens MQTT
4. ✅ Dashboard Node-RED funcionando (se implementou)
5. ✅ Foto/vídeo do hardware montado
6. ✅ Vídeo mostrando:
   - LED acendendo ao detectar presença
   - Valores mudando no dashboard ao interagir com sensores

---

## 🎯 Checklist de Entrega

- [ ] Código funcional no GitHub
- [ ] README.md documentado
- [ ] Flow Node-RED exportado (node-red-flow.json)
- [ ] Vídeo de demonstração (2-3 minutos)
- [ ] Documentação técnica (arquitetura FreeRTOS)
- [ ] Diagrama de conexões (Fritzing/esquemático)
- [ ] Evidências de testes (prints/vídeos)

---

**Boa sorte! 🚀**

**Dúvidas?** Verifique o código, está bem comentado!
