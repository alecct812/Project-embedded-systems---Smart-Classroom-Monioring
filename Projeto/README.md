# Sistema de Monitoramento de Sala de Aula Inteligente

## 📋 Descrição do Projeto

Sistema IoT usando ESP32 para monitorar condições ambientais e ocupação de uma sala de aula, com integração MQTT e Node-RED para dashboard em tempo real.

**Diferencial:** Usa FreeRTOS com filas e tasks para processamento paralelo e eficiente, com detecção automática de sensores!

## 🔧 Componentes Necessários

Você pode conectar TODOS os sensores em UM único ESP32, ou distribuir entre vários:

- ESP32 (1 ou mais)
- Sensor DHT22 (temperatura e umidade)
- 2x LDR (sensores de luminosidade)
- 2x Resistor 10kΩ (para os LDRs)
- Sensor PIR (movimento/presença)
- LED
- Resistor 220Ω (para o LED)
- Cabos jumper

## 🔌 Esquema de Ligações

### Pinos Configurados:
- **DHT22 DATA** → GPIO 4
- **LDR 1** → GPIO 34 (com resistor 10kΩ ao GND)
- **LDR 2** → GPIO 35 (com resistor 10kΩ ao GND)
- **PIR OUT** → GPIO 25
- **LED** → GPIO 32 (com resistor 220Ω)

### Alimentação:
- DHT22 VCC → 3.3V
- PIR VCC → 5V
- LDRs: um terminal ao 3.3V
- Todos GND conectados

## 🚀 Como Funciona

### Detecção Automática de Sensores
O sistema detecta automaticamente quais sensores estão conectados ao iniciar! Não precisa descomentar nada ou mudar código.

### Arquitetura FreeRTOS
O sistema usa **tasks paralelas** para máxima eficiência:

- **Task DHT22** (Core 0): Lê temperatura e umidade a cada 5s
- **Task LDR** (Core 0): Lê luminosidade dos 2 sensores a cada 5s
- **Task PIR** (Core 0): Monitora presença constantemente (500ms)
- **Task MQTT** (Core 1): Publica dados e mantém conexão

### Comunicação via Filas
As tasks de leitura enviam dados para **filas FreeRTOS**, e a task MQTT consome e publica no broker.

## 📡 Tópicos MQTT

Configure estes tópicos no Node-RED:

| Tópico | Descrição | Formato |
|--------|-----------|---------|
| `sala/temperatura` | Temperatura em °C | Float ("23.50") |
| `sala/umidade` | Umidade relativa % | Float ("55.20") |
| `sala/luminosidade1` | Luminosidade sensor 1 | Float ("75.00") |
| `sala/luminosidade2` | Luminosidade sensor 2 | Float ("72.50") |
| `sala/presenca` | Status presença | "DETECTADA" / "NAO_DETECTADA" |
| `sala/ocupacao` | Status ocupação | "OCUPADA" / "VAZIA" |
| `sala/alertas` | Alertas de condições | String |
| `sala/sugestao_ac` | Sugestão AC | "LIGAR" / "DESLIGAR" |

## ⚙️ Instalação e Uso

### 1. Configurar Credenciais
Edite no `main.cpp`:
```cpp
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
const char* mqtt_server = "IP_DO_BROKER";  // IP do Node-RED
```

### 2. Conectar Sensores
Conecte os sensores que você tem disponível. O sistema detecta automaticamente!

### 3. Build e Upload
No PlatformIO:
1. Clique em **Build** (ícone ✓)
2. Clique em **Upload** (ícone →)
3. Abra o **Serial Monitor** para ver logs

### 4. Configurar Node-RED
No Node-RED:
1. Adicione node **MQTT Broker** com o IP do seu broker
2. Adicione nodes **MQTT In** para cada tópico
3. Crie o dashboard com gauges, gráficos e indicadores

## 📊 Funcionalidades

### ✅ Monitoramento em Tempo Real
- Temperatura ambiente
- Umidade relativa do ar
- Luminosidade (até 2 pontos diferentes)
- Detecção de presença
- Status de ocupação da sala

### ⚠️ Alertas Automáticos
- Temperatura fora da faixa ideal (20-25°C)
- Umidade fora da faixa ideal (40-60%)

### 💡 Recursos Inteligentes
- LED acende automaticamente quando detecta presença
- Sugestão de ligar/desligar ar-condicionado
- Logs com emojis para fácil visualização

## 🎯 Vantagens da Arquitetura FreeRTOS

✨ **Processamento Paralelo**: Múltiplas tasks executando simultaneamente
✨ **Uso de 2 Cores**: Distribui carga entre os 2 cores do ESP32
✨ **Filas Seguras**: Comunicação thread-safe entre tasks
✨ **Detecção Automática**: Não precisa recompilar para diferentes configurações
✨ **Escalável**: Fácil adicionar novos sensores
✨ **Eficiente**: Usa menos memória e CPU

## 🔍 Logs do Serial Monitor

Ao iniciar, você verá:
```
╔════════════════════════════════════════════════╗
║  Sistema de Monitoramento de Sala Inteligente ║
║              GRUPO 10 - ESP32                  ║
╚════════════════════════════════════════════════╝

=== Detectando sensores conectados ===
✓ DHT22 detectado (GPIO 4)
✓ LDR 1 detectado (GPIO 34)
✓ LDR 2 detectado (GPIO 35)
✓ PIR configurado (GPIO 25)

=== Criando Tasks FreeRTOS ===
✓ Task DHT22 criada no Core 0
✓ Task LDR criada no Core 0
✓ Task PIR criada no Core 0
✓ Task MQTT criada no Core 1

🚀 Sistema iniciado com sucesso!
📡 Monitorando sensores...
```

## 📱 Dashboard Node-RED (Sugestão)

Configure no Node-RED:
- **Gauges**: Temperatura, umidade em tempo real
- **Gráficos de linha**: Histórico de temperatura/umidade
- **Barras**: Níveis de luminosidade
- **Indicador**: Status de ocupação (verde/vermelho)
- **Alertas**: Notificações visuais
- **Card AC**: Mostra sugestão de ar-condicionado

## 🔧 Troubleshooting

**Sensor não detectado?**
- Verifique as conexões físicas
- Confirme o pino correto no código
- Para LDR: valor deve estar entre 0-4095 (não totalmente 0 ou 4095)

**Não conecta no MQTT?**
- Verifique o IP do broker
- Confirme que porta 1883 está aberta
- Teste conexão de rede

**Task não criada?**
- Sensor pode não ter sido detectado
- Verifique logs no Serial Monitor

## 👥 Grupo 10

Projeto desenvolvido como parte da disciplina de Sistemas Embarcados.
