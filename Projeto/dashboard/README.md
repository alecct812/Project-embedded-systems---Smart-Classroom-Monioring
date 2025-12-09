# 🏫 Smart Classroom Monitor - Dashboard

Dashboard web em tempo real para o Sistema de Monitoramento de Sala de Aula Inteligente.

## 🎯 Funcionalidades

- ✅ **Monitoramento em Tempo Real** via MQTT
- 📊 **Gráficos Históricos** de temperatura e umidade
- 🎨 **Interface Moderna** com TailwindCSS
- ⚠️ **Sistema de Alertas** visual
- 💡 **Sugestões Inteligentes** (ar-condicionado, iluminação)
- 📱 **Design Responsivo** para qualquer dispositivo
- 🔌 **Conexão WebSocket** com broker MQTT

## 🛠️ Tecnologias

- **React 18** - Framework UI
- **Vite** - Build tool rápida
- **TailwindCSS** - Estilização
- **MQTT.js** - Comunicação MQTT via WebSocket
- **Recharts** - Gráficos interativos

## 📋 Pré-requisitos

Antes de começar, certifique-se de ter:

1. **Node.js** (versão 18 ou superior)
2. **npm** ou **yarn**
3. **Broker MQTT** rodando com suporte a WebSocket (porta 9001)

### ⚙️ Configurando o Broker MQTT com WebSocket

O dashboard usa WebSocket para conectar ao broker MQTT. Você precisa:

**Opção 1: Mosquitto (Recomendado)**

Instale o Mosquitto e adicione ao arquivo `mosquitto.conf`:

```conf
# Porta padrão MQTT
listener 1883

# WebSocket para navegador
listener 9001
protocol websockets
```

Reinicie o Mosquitto:

```bash
mosquitto -c mosquitto.conf
```

**Opção 2: HiveMQ Broker Público (para testes)**

Se preferir usar um broker público para testes, edite `src/App.jsx`:

```javascript
const BROKER_URL = 'ws://broker.hivemq.com:8000/mqtt';
```

## 🚀 Como Executar

### 1. Instalar Dependências

Entre na pasta do dashboard:

```bash
cd dashboard
npm install
```

### 2. Configurar IP do Broker

Edite o arquivo `src/App.jsx` e ajuste o IP do broker MQTT:

```javascript
// Linha 9 do App.jsx
const BROKER_URL = 'ws://SEU_IP_AQUI:9001';
```

**Importante**: Use o mesmo IP configurado no ESP32 (arquivo `main.cpp`).

### 3. Executar o Dashboard

```bash
npm run dev
```

O dashboard abrirá automaticamente em:
- **Local**: http://localhost:3000
- **Rede**: http://seu-ip:3000

### 4. Build para Produção

Para criar uma versão otimizada:

```bash
npm run build
```

Os arquivos ficarão na pasta `dist/`.

## 📊 Dados Monitorados

O dashboard subscreve aos seguintes tópicos MQTT:

| Tópico | Descrição | Exemplo |
|--------|-----------|---------|
| `sala/temperatura` | Temperatura em °C | `26.5` |
| `sala/umidade` | Umidade em % | `57.0` |
| `sala/luminosidade` | Luminosidade em % | `60.0` |
| `sala/presenca` | Status de presença | `DETECTADA` |
| `sala/ocupacao` | Status da sala | `OCUPADA` |
| `sala/alertas` | Alertas do sistema | `ALERTA: Temperatura fora do ideal` |
| `sala/sugestao_ac` | Sugestão AC | `LIGAR` ou `DESLIGAR` |

## 🎨 Componentes

O projeto é modular e bem organizado:

```
dashboard/
├── src/
│   ├── components/          # Componentes React
│   │   ├── MetricCard.jsx   # Card de métrica
│   │   ├── StatusCard.jsx   # Card de status
│   │   ├── AlertPanel.jsx   # Painel de alertas
│   │   ├── TemperatureChart.jsx  # Gráfico temperatura
│   │   ├── HumidityChart.jsx     # Gráfico umidade
│   │   └── ConnectionStatus.jsx  # Status conexão
│   ├── hooks/
│   │   └── useMqttData.js   # Hook customizado MQTT
│   ├── App.jsx              # Componente principal
│   └── main.jsx             # Entry point
```

## 🔧 Configurações Avançadas

### Alterar Porta do Servidor

Edite `vite.config.js`:

```javascript
export default defineConfig({
  server: {
    port: 3000,  // Altere aqui
    host: true
  }
})
```

### Ajustar Faixas Ideais

Os valores ideais estão definidos em:
- **ESP32**: `main.cpp` (linhas 58-61)
- **Dashboard**: Lógica nos componentes

## 🐛 Troubleshooting

### Dashboard não conecta ao MQTT

1. Verifique se o broker está rodando
2. Confirme que o WebSocket está habilitado (porta 9001)
3. Verifique o IP no arquivo `App.jsx`
4. Teste a conexão: `telnet seu-ip 9001`

### Dados não aparecem

1. Certifique-se que o ESP32 está conectado ao broker
2. Verifique o monitor serial do ESP32
3. Confirme que os tópicos estão corretos
4. Abra o console do navegador (F12) para ver logs

### Gráficos não aparecem

1. Aguarde alguns segundos para coletar dados
2. Verifique se os dados estão chegando (console do navegador)
3. Recarregue a página

## 📱 Screenshots

O dashboard exibe:
- 📊 Cards com métricas em tempo real
- 📈 Gráficos históricos interativos
- ⚠️ Alertas visuais destacados
- 💡 Sugestões contextuais
- 🔌 Indicador de conexão

## 🤝 Contribuindo

Desenvolvido pelo Grupo 10 - CESAR School 2025

## 📄 Licença

Projeto educacional - Sistema de Monitoramento IoT

