# Dashboard - Sistema de Monitoramento de Sala Inteligente

Dashboard web moderno e responsivo para visualizar dados em tempo real do sistema de monitoramento de sala de aula.

## 🚀 Tecnologias Utilizadas

- **React** - Framework JavaScript
- **Vite** - Build tool rápida
- **Material-UI (MUI)** - Biblioteca de componentes React
- **Recharts** - Gráficos interativos
- **MQTT.js** - Cliente MQTT para conexão em tempo real

## 📋 Pré-requisitos

- Node.js 16+ instalado
- Broker MQTT com suporte a WebSocket (ou proxy configurado)

## ⚙️ Instalação

1. Instale as dependências:
```bash
npm install
```

2. Configure o broker MQTT no arquivo `src/config.js`:
```javascript
export const MQTT_CONFIG = {
  wsUrl: 'ws://SEU_IP:9001', // Ajuste conforme necessário
  // ...
};
```

## 🔧 Configuração do Broker MQTT para WebSocket

Como navegadores não podem fazer conexões TCP diretas, você precisa configurar WebSocket no seu broker MQTT.

### Opção 1: Mosquitto com WebSocket

1. Instale o Mosquitto:
```bash
# macOS
brew install mosquitto

# Linux
sudo apt-get install mosquitto mosquitto-clients
```

2. Configure o arquivo `mosquitto.conf`:
```
listener 1883
protocol mqtt

listener 9001
protocol websockets
```

3. Inicie o Mosquitto:
```bash
mosquitto -c mosquitto.conf
```

### Opção 2: Usar Node-RED com WebSocket

Se você já usa Node-RED, pode adicionar um node de WebSocket que faça proxy para o MQTT.

### Opção 3: Broker Público (para testes)

Para testes rápidos, você pode usar um broker público:
```javascript
wsUrl: 'wss://broker.emqx.io:8084/mqtt'
```

## 🏃 Como Executar

1. Inicie o servidor de desenvolvimento:
```bash
npm run dev
```

2. Abra o navegador em `http://localhost:5173`

3. O dashboard se conectará automaticamente ao broker MQTT e começará a receber dados em tempo real.

## 📊 Funcionalidades

- ✅ **Monitoramento em Tempo Real**: Visualização instantânea de temperatura, umidade, luminosidade e presença
- ✅ **Gráficos de Histórico**: Gráficos de linha mostrando tendências dos últimos 30 pontos
- ✅ **Alertas Inteligentes**: Notificações quando valores estão fora da faixa ideal
- ✅ **Sugestões de Ação**: Recomendações automáticas para ligar/desligar ar-condicionado
- ✅ **Status de Conexão**: Indicador visual do status da conexão MQTT
- ✅ **Design Responsivo**: Funciona perfeitamente em desktop, tablet e mobile

## 🎨 Componentes

- **MetricCard**: Cards visuais para cada métrica (temperatura, umidade, luminosidade)
- **PresenceCard**: Card especial para status de ocupação da sala
- **AlertCard**: Lista de alertas do sistema
- **SuggestionCard**: Sugestões de ações (ex: ligar/desligar AC)
- **LineChart**: Gráficos de histórico usando Recharts
- **ConnectionStatus**: Indicador de status da conexão MQTT

## 🔌 Tópicos MQTT

O dashboard subscreve aos seguintes tópicos:
- `sala/temperatura` - Temperatura em °C
- `sala/umidade` - Umidade em %
- `sala/luminosidade` - Luminosidade em %
- `sala/presenca` - Status de presença
- `sala/ocupacao` - Status de ocupação
- `sala/alertas` - Mensagens de alerta
- `sala/sugestao_ac` - Sugestões de ar-condicionado

## 🛠️ Build para Produção

Para criar uma versão otimizada para produção:

```bash
npm run build
```

Os arquivos serão gerados na pasta `dist/`. Você pode servir esses arquivos com qualquer servidor web estático (nginx, Apache, etc).

## 📝 Notas Importantes

- O dashboard precisa de um broker MQTT com suporte a WebSocket
- A porta padrão para MQTT WebSocket é 9001
- Certifique-se de que o firewall permite conexões na porta configurada
- Para produção, considere usar HTTPS/WSS para segurança

## 🐛 Troubleshooting

**Dashboard não conecta ao MQTT?**
- Verifique se o broker está rodando
- Confirme que o WebSocket está habilitado na porta correta
- Verifique o IP/URL no arquivo `config.js`
- Abra o console do navegador (F12) para ver erros detalhados

**Dados não aparecem?**
- Verifique se o ESP32 está publicando nos tópicos corretos
- Confirme que os tópicos no `config.js` correspondem aos do ESP32
- Verifique a conexão do ESP32 com o broker MQTT

## 📄 Licença

Este projeto foi desenvolvido como parte da disciplina de Sistemas Embarcados - Grupo 10.
