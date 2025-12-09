# 🏫 Smart Classroom Monitoring System

Sistema de Monitoramento de Sala Inteligente desenvolvido pelo **Grupo 10** para a disciplina de Sistemas Embarcados - CESAR School 2025.

## 📋 Índice

1. [Visão Geral](#-visão-geral)
2. [Arquitetura do Sistema](#-arquitetura-do-sistema)
3. [Estrutura do Projeto](#-estrutura-do-projeto)
4. [Hardware Necessário](#-hardware-necessário)
5. [Requisitos de Software](#-requisitos-de-software)
6. [Passo a Passo: Configuração Completa](#-passo-a-passo-configuração-completa)
7. [Tópicos MQTT](#-tópicos-mqtt)
8. [Solução de Problemas](#-solução-de-problemas)

---

## 🎯 Visão Geral

Sistema embarcado que monitora em tempo real as condições ambientais de uma sala de aula:

- **Temperatura e Umidade** (DHT11)
- **Luminosidade** (LDR)
- **Contagem de pessoas** (2x TCRT-5000)
- **Indicador de ocupação** (LED)

Os dados são transmitidos via **MQTT** para um **dashboard web** em tempo real.

---

## 🏗 Arquitetura do Sistema

```
┌─────────────────────────────────────────────────────────────────────┐
│                           SALA DE AULA                              │
│  ┌──────────────┐                                                   │
│  │    ESP32     │──── DHT11 (Temp/Umidade)                         │
│  │  (FreeRTOS)  │──── LDR (Luminosidade)                           │
│  │              │──── TCRT-5000 x2 (Entrada/Saída)                 │
│  │              │──── LED (Indicador)                              │
│  └──────┬───────┘                                                   │
│         │ WiFi                                                      │
└─────────┼───────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      SERVIDOR (Raspberry Pi / PC)                   │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐ │
│  │   Mosquitto     │◄───│  Proxy MQTT     │◄───│   Dashboard     │ │
│  │   (Docker)      │    │  (WebSocket)    │    │   (React)       │ │
│  │   Port: 1883    │    │   Port: 8080    │    │   Port: 3000    │ │
│  └─────────────────┘    └─────────────────┘    └─────────────────┘ │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 📁 Estrutura do Projeto

```
├── README.md                 # Este arquivo
├── /docs                     # Relatório PDF (ABNT2) + imagens
├── /esp32-esp8266           # Firmware do microcontrolador
│   ├── src/
│   │   └── main.cpp         # Código principal (FreeRTOS)
│   ├── platformio.ini       # Configuração PlatformIO
│   ├── include/             # Headers
│   ├── lib/                 # Bibliotecas locais
│   └── test/                # Testes
├── /raspberry-pi            # Broker MQTT + Dashboard
│   ├── docker-compose.yml   # Container Mosquitto
│   ├── mosquitto/           # Configurações do broker
│   ├── mqtt-proxy-server.js # Proxy WebSocket
│   ├── package.json         # Dependências do proxy
│   └── dashboard/           # Aplicação React
│       ├── src/
│       ├── package.json
│       └── vite.config.js
└── /schematics              # Diagramas eletrônicos
```

---

## 🔧 Hardware Necessário

### Lista de Componentes

| Componente | Quantidade | Descrição | Conexão ESP32 |
|------------|------------|-----------|---------------|
| ESP32 DevKit | 1 | Microcontrolador dual-core | - |
| DHT11 | 1 | Sensor de temperatura e umidade | GPIO 4 |
| LDR + Resistor 10kΩ | 1 | Sensor de luminosidade | GPIO 32 |
| TCRT5000 | 2 | Sensor infravermelho reflexivo | GPIO 13, GPIO 14 |
| LED + Resistor 220Ω | 1 | Indicador de ocupação | GPIO 2 |
| Protoboard | 1 | Para montagem | - |
| Jumpers | Vários | Conexões | - |
| Cabo USB | 1 | Programação e alimentação | - |

### Esquema de Conexão

```
ESP32 DevKit V1
┌──────────────────────────────────┐
│                                  │
│  3.3V ────────► DHT11 (VCC)      │
│  GND  ────────► DHT11 (GND)      │
│  GPIO4 ───────► DHT11 (DATA)     │
│                                  │
│  3.3V ────────► LDR (divisor)    │
│  GND  ────────► Resistor 10kΩ    │
│  GPIO32 ──────► Ponto médio      │
│                                  │
│  3.3V ────────► TCRT1 (VCC)      │
│  GND  ────────► TCRT1 (GND)      │
│  GPIO13 ──────► TCRT1 (OUT)      │
│                                  │
│  3.3V ────────► TCRT2 (VCC)      │
│  GND  ────────► TCRT2 (GND)      │
│  GPIO14 ──────► TCRT2 (OUT)      │
│                                  │
│  GPIO2 ───────► LED (com R 220Ω) │
│  GND  ────────► LED (cátodo)     │
│                                  │
└──────────────────────────────────┘
```

---

## 💻 Requisitos de Software

### No Computador de Desenvolvimento

| Software | Versão | Função |
|----------|--------|--------|
| [VS Code](https://code.visualstudio.com/) | Última | IDE de desenvolvimento |
| [PlatformIO Extension](https://platformio.org/) | Última | Compilar/Upload ESP32 |
| [Docker Desktop](https://www.docker.com/products/docker-desktop/) | Última | Rodar Mosquitto |
| [Node.js](https://nodejs.org/) | 18+ | Dashboard e Proxy |
| [Git](https://git-scm.com/) | Última | Controle de versão |

### Verificar Instalações

Abra o terminal e execute:

```bash
# Verificar Node.js
node --version
# Deve mostrar: v18.x.x ou superior

# Verificar npm
npm --version
# Deve mostrar: 9.x.x ou superior

# Verificar Docker
docker --version
# Deve mostrar: Docker version 24.x.x ou superior

# Verificar Docker Compose
docker compose version
# Deve mostrar: Docker Compose version v2.x.x
```

---

## 🚀 Passo a Passo: Configuração Completa

### ETAPA 1: Clonar o Repositório

```bash
# Clone o projeto
git clone <URL_DO_REPOSITÓRIO>

# Entre na pasta do projeto
cd Project-embedded-systems---Smart-Classroom-Monioring
```

---

### ETAPA 2: Configurar o Firmware do ESP32

#### 2.1 Abrir no VS Code com PlatformIO

```bash
# Abra a pasta do firmware no VS Code
code esp32-esp8266
```

#### 2.2 Configurar WiFi e IP do Broker

Abra o arquivo `esp32-esp8266/src/main.cpp` e altere as linhas 17-20:

```cpp
// ========== CONFIGURAÇÕES ==========
const char *ssid = "NOME_DA_SUA_REDE_WIFI";      // ← Altere aqui
const char *password = "SENHA_DO_SEU_WIFI";     // ← Altere aqui
const char *mqtt_server = "IP_DO_SEU_SERVIDOR"; // ← IP onde roda o Mosquitto
const uint16_t MQTT_PORT = 1883;
```

**Como descobrir o IP do servidor:**
- Windows: `ipconfig` (procure "IPv4 Address")
- Linux/Mac: `ifconfig` ou `ip addr`

#### 2.3 Conectar o ESP32

1. Conecte o ESP32 via cabo USB
2. Verifique se a porta foi detectada:
   - Windows: Aparece como `COM3`, `COM4`, etc.
   - Linux: Aparece como `/dev/ttyUSB0` ou `/dev/ttyACM0`
   - Mac: Aparece como `/dev/cu.usbserial-XXXX`

#### 2.4 Compilar e Fazer Upload

No VS Code com PlatformIO:

1. Clique no ícone **PlatformIO** na barra lateral (formiga)
2. Clique em **Build** (✓) para compilar
3. Clique em **Upload** (→) para enviar ao ESP32
4. Clique em **Serial Monitor** para ver os logs

**Saída esperada no Serial Monitor:**
```
╔═══════════════════════════════════╗
║  Sala Inteligente - Grupo 10     ║
╚═══════════════════════════════════╝

╔═══ DETECTANDO SENSORES ═══╗
✓ DHT11
✓ LDR
✓ TCRT Sensor 1 (Externo)
✓ TCRT Sensor 2 (Interno)
✓ LED
╚═══════════════════════════╝

Conectando ao Wi-Fi: SuaRede
....
✓ WiFi conectado!
IP: 192.168.1.XXX

Conectando MQTT... ✓ Conectado!
🚀 Sistema iniciado!
🐳 Broker: 192.168.1.YYY:1883
```

---

### ETAPA 3: Iniciar o Broker MQTT (Mosquitto)

#### 3.1 Navegar para a pasta do Raspberry Pi

```bash
cd raspberry-pi
```

#### 3.2 Iniciar o Mosquitto com Docker

```bash
# Inicia o container em background
docker compose up -d

# Verificar se está rodando
docker ps
```

**Saída esperada:**
```
CONTAINER ID   IMAGE                    PORTS                    NAMES
abc123def456   eclipse-mosquitto:latest 0.0.0.0:1883->1883/tcp   mosquitto
                                        0.0.0.0:9001->9001/tcp
```

#### 3.3 Testar o Broker (Opcional)

```bash
# Ver logs do Mosquitto
docker logs mosquitto -f

# Em outro terminal, subscrever a um tópico para teste
docker exec -it mosquitto mosquitto_sub -t "sala/#" -v
```

---

### ETAPA 4: Iniciar o Dashboard

#### 4.1 Instalar Dependências do Dashboard

```bash
# Ainda na pasta raspberry-pi
cd dashboard

# Instalar dependências
npm install
```

#### 4.2 Configurar o Broker no Dashboard

Edite o arquivo `raspberry-pi/dashboard/src/config.js`:

```javascript
export const MQTT_CONFIG = {
  // Para conexão direta via WebSocket (porta 9001)
  brokerUrl: 'ws://localhost:9001',
  
  // OU se estiver em outra máquina:
  // brokerUrl: 'ws://IP_DO_SERVIDOR:9001',
  
  // ... resto das configurações
};
```

#### 4.3 Iniciar o Dashboard

```bash
# Iniciar servidor de desenvolvimento
npm run dev
```

**Saída esperada:**
```
  VITE v5.0.8  ready in 500 ms

  ➜  Local:   http://localhost:3000/
  ➜  Network: http://192.168.1.XXX:3000/
```

#### 4.4 Acessar o Dashboard

Abra o navegador em: **http://localhost:3000**

---

### ETAPA 5: Testar o Sistema Completo

#### Checklist de Verificação

- [ ] ESP32 conectado ao WiFi (ver Serial Monitor)
- [ ] ESP32 conectado ao MQTT (mensagem "Conectado!")
- [ ] Mosquitto rodando (docker ps mostra container ativo)
- [ ] Dashboard aberto no navegador
- [ ] Indicador "Conectado" verde no dashboard

#### Testes Funcionais

1. **Temperatura/Umidade**: Os valores devem aparecer nos cards
2. **Luminosidade**: Cubra/descubra o LDR e veja a variação
3. **Entrada/Saída**: Passe a mão nos sensores TCRT na sequência:
   - TCRT1 → TCRT2 = Entrada (contador aumenta)
   - TCRT2 → TCRT1 = Saída (contador diminui)
4. **LED**: Deve acender quando há pessoas na sala

---

## 📡 Tópicos MQTT

O ESP32 publica nos seguintes tópicos:

| Tópico | Tipo | Descrição | Exemplo |
|--------|------|-----------|---------|
| `sala/status` | String | Status da conexão | "ESP32 Online" |
| `sala/temperatura` | Float | Temperatura em °C | "23.50" |
| `sala/umidade` | Float | Umidade relativa % | "65.00" |
| `sala/luminosidade` | Float | Luminosidade % | "78.00" |
| `sala/alertas` | String | Alertas do sistema | "Temperatura fora do ideal" |
| `sala/entrada` | String | Evento de entrada | "DETECTADA" |
| `sala/saida` | String | Evento de saída | "DETECTADA" |
| `sala/pessoas` | Int | Número de pessoas | "3" |
| `sala/ocupacao` | String | Estado da sala | "OCUPADA" ou "VAZIA" |

### Monitorar Tópicos via Terminal

```bash
# Subscrever a todos os tópicos da sala
docker exec -it mosquitto mosquitto_sub -t "sala/#" -v
```

---

## 🔧 Solução de Problemas

### ESP32 não conecta ao WiFi

**Sintoma:** Serial Monitor mostra pontos infinitos `...........`

**Soluções:**
1. Verifique SSID e senha (case-sensitive)
2. Certifique-se que a rede é 2.4GHz (ESP32 não suporta 5GHz)
3. Aproxime o ESP32 do roteador
4. Reinicie o roteador

---

### ESP32 não conecta ao MQTT

**Sintoma:** `Conectando MQTT... ✗ Falha (-2)`

**Códigos de erro:**
- `-1`: Timeout de conexão
- `-2`: Conexão recusada (broker offline)
- `-4`: Servidor não encontrado

**Soluções:**
1. Verifique se o Mosquitto está rodando: `docker ps`
2. Verifique se o IP está correto no código
3. Teste conectividade: `ping IP_DO_SERVIDOR`
4. Verifique firewall/antivírus

---

### Dashboard não recebe dados

**Sintoma:** Dashboard mostra "Desconectado" ou valores não atualizam

**Soluções:**
1. Verifique se a porta 9001 está aberta no Mosquitto
2. Confira o arquivo `mosquitto/config/mosquitto.conf`:
   ```
   listener 9001
   protocol websockets
   allow_anonymous true
   ```
3. Reinicie o Mosquitto: `docker compose restart`
4. Verifique console do navegador (F12) para erros

---

### Sensor DHT11 retorna NaN

**Sintoma:** Serial mostra `✗ DHT11` na inicialização

**Soluções:**
1. Verifique conexões (VCC, GND, DATA)
2. Adicione resistor pull-up de 10kΩ entre DATA e VCC
3. Verifique se o pino está correto (GPIO 4)
4. Substitua o sensor (pode estar danificado)

---

### Contagem de pessoas incorreta

**Sintoma:** Entradas/saídas não são detectadas corretamente

**Soluções:**
1. Ajuste a distância entre os sensores TCRT (5-10cm ideal)
2. Aumente o `TIMEOUT_SEQUENCIA` no código (padrão: 2000ms)
3. Verifique se os sensores estão na ordem correta (1=externo, 2=interno)
4. Ajuste a altura dos sensores (nível da cintura funciona melhor)

---

## 📄 Comandos Úteis

```bash
# === DOCKER ===
docker compose up -d          # Iniciar Mosquitto
docker compose down           # Parar Mosquitto
docker compose restart        # Reiniciar
docker logs mosquitto -f      # Ver logs

# === MQTT (TESTES) ===
# Subscrever a tópicos
docker exec -it mosquitto mosquitto_sub -t "sala/#" -v

# Publicar mensagem de teste
docker exec -it mosquitto mosquitto_pub -t "sala/teste" -m "Hello"

# === DASHBOARD ===
cd raspberry-pi/dashboard
npm install                   # Instalar dependências
npm run dev                   # Iniciar desenvolvimento
npm run build                 # Build para produção

# === PLATFORMIO (ESP32) ===
# Use os botões do VS Code ou:
pio run                       # Compilar
pio run --target upload       # Upload
pio device monitor            # Serial Monitor
```

---

## 👥 Equipe - Grupo 10

Projeto desenvolvido para a disciplina de Sistemas Embarcados - CESAR School 2025

---

## 📚 Tecnologias Utilizadas

- **Microcontrolador:** ESP32 (Dual-Core Xtensa LX6, 240MHz)
- **RTOS:** FreeRTOS
- **Protocolo:** MQTT
- **Broker:** Eclipse Mosquitto (Docker)
- **Frontend:** React + Vite + TailwindCSS
- **Gráficos:** Recharts
- **Containerização:** Docker Compose
