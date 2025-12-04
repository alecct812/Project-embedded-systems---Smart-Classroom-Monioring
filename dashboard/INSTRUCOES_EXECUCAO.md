# 📋 Instruções para Executar o Dashboard

## ✅ Pré-requisitos

1. **Node.js instalado** (versão 16 ou superior)
   - Verifique: `node --version`
   - Se não tiver, baixe em: https://nodejs.org/

2. **Broker MQTT rodando**
   - O ESP32 precisa estar conectado e publicando dados
   - Broker MQTT deve estar acessível no IP: `172.26.70.17:1883`

## 🚀 Passo a Passo

### Passo 1: Navegar até a pasta do dashboard

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
```

### Passo 2: Instalar dependências (primeira vez apenas)

```bash
npm install
```

Este comando instala todas as bibliotecas necessárias (React, Material-UI, MQTT, etc).

**⏱️ Tempo estimado:** 1-2 minutos

### Passo 3: Configurar conexão MQTT

Edite o arquivo `src/config.js` e verifique/ajuste o IP do broker:

```javascript
wsUrl: 'ws://172.26.70.17:9001',  // Ajuste se necessário
```

**⚠️ IMPORTANTE:** 
- Navegadores precisam de WebSocket (não TCP direto)
- Se seu broker não tem WebSocket, veja a opção alternativa abaixo

### Passo 4: Iniciar o servidor de desenvolvimento

```bash
npm run dev
```

Você verá uma saída similar a:

```
  VITE v7.2.4  ready in 500 ms

  ➜  Local:   http://localhost:5173/
  ➜  Network: use --host to expose
```

### Passo 5: Abrir no navegador

Abra seu navegador e acesse:

```
http://localhost:5173
```

O dashboard deve aparecer e tentar conectar ao MQTT automaticamente.

## 🔧 Se o Broker Não Tiver WebSocket

Se você receber erro de conexão, seu broker pode não ter WebSocket habilitado. Use o proxy server:

### Opção A: Usar Proxy Server (Recomendado)

1. **Em um terminal separado**, execute o proxy:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
node mqtt-proxy-server.js
```

Você verá:
```
🚀 Servidor Proxy MQTT iniciado na porta 8080
✅ Conectado ao broker MQTT
```

2. **Edite `src/config.js`** e altere para:

```javascript
wsUrl: 'ws://localhost:8080',  // Usa o proxy local
```

3. **Mantenha ambos rodando**:
   - Terminal 1: `node mqtt-proxy-server.js` (proxy)
   - Terminal 2: `npm run dev` (dashboard)

### Opção B: Configurar Mosquitto com WebSocket

Se você usa Mosquitto como broker:

1. **Instale Mosquitto** (se ainda não tiver):
```bash
# macOS
brew install mosquitto

# Linux
sudo apt-get install mosquitto mosquitto-clients
```

2. **Crie/edite** `mosquitto.conf`:
```
listener 1883
protocol mqtt

listener 9001
protocol websockets
```

3. **Inicie Mosquitto**:
```bash
mosquitto -c mosquitto.conf
```

4. **Use no config.js**:
```javascript
wsUrl: 'ws://172.26.70.17:9001',
```

## ✅ Verificando se Está Funcionando

1. **Status de Conexão**: No topo direito do dashboard, deve aparecer "Conectado" (verde)

2. **Dados Aparecendo**: Os cards devem mostrar valores de temperatura, umidade, etc.

3. **Console do Navegador**: Pressione `F12` e veja se há erros. Deve aparecer:
   ```
   ✅ Conectado ao MQTT
   ```

## 🐛 Problemas Comuns

### ❌ "Erro ao conectar ao MQTT"

**Solução:**
- Verifique se o broker está rodando
- Confirme o IP no `config.js`
- Tente usar o proxy server (Opção A acima)

### ❌ "Dados não aparecem"

**Solução:**
- Verifique se o ESP32 está conectado e publicando
- Abra o console do navegador (F12) e veja se há mensagens MQTT
- Confirme que os tópicos no `config.js` correspondem aos do ESP32

### ❌ "npm: command not found"

**Solução:**
- Instale Node.js: https://nodejs.org/
- Reinicie o terminal após instalar

### ❌ "Porta 5173 já em uso"

**Solução:**
- Feche outros processos usando a porta
- Ou use outra porta: `npm run dev -- --port 3000`

## 📊 O Que Você Deve Ver

Quando tudo estiver funcionando:

- ✅ **Cards com valores**: Temperatura, Umidade, Luminosidade
- ✅ **Status da Sala**: Ocupada/Vazia com indicador visual
- ✅ **Gráficos**: Histórico dos últimos 30 pontos
- ✅ **Alertas**: Notificações quando valores estão fora do ideal
- ✅ **Sugestões**: Recomendações de ações (ex: ligar AC)

## 🛑 Parar o Servidor

Para parar o servidor, pressione `Ctrl + C` no terminal.

## 📝 Comandos Úteis

```bash
# Instalar dependências
npm install

# Iniciar servidor de desenvolvimento
npm run dev

# Criar build para produção
npm run build

# Visualizar build de produção
npm run preview
```

## 🆘 Precisa de Ajuda?

1. Verifique o console do navegador (F12)
2. Verifique os logs do terminal onde roda `npm run dev`
3. Confirme que o ESP32 está publicando dados nos tópicos corretos
4. Teste a conexão MQTT com um cliente como MQTTX

---

**🎉 Pronto!** Seu dashboard deve estar rodando e recebendo dados em tempo real!

