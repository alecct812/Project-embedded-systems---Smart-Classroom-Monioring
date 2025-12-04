# 🚀 Guia de Execução - Dashboard em Tempo Real

Este guia explica como executar o dashboard para receber dados do ESP32 em tempo real via MQTT.

## 📋 Pré-requisitos

1. **Node.js instalado** (versão 16 ou superior)
2. **ESP32 rodando e publicando dados** no broker MQTT `172.26.70.17:1883`
3. **Broker MQTT Mosquitto** rodando e acessível

## 🔧 Passo a Passo

### Passo 1: Instalar Dependências do Proxy Server

O proxy server precisa das bibliotecas `ws` e `mqtt`:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
npm install ws mqtt
```

### Passo 2: Iniciar o Proxy Server MQTT

**Em um terminal separado**, execute:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
node mqtt-proxy-server.js
```

Você deve ver:

```
🚀 Servidor Proxy MQTT iniciado na porta 8080
📡 Broker MQTT: mqtt://172.26.70.17:1883
🌐 Dashboard deve usar: ws://localhost:8080

✅ Cliente WebSocket conectado: dashboard_...
✅ dashboard_... conectado ao broker MQTT
📥 dashboard_... subscreveu em sala/temperatura
📥 dashboard_... subscreveu em sala/umidade
...
```

**⚠️ IMPORTANTE:** Mantenha este terminal aberto enquanto o dashboard estiver rodando!

### Passo 3: Iniciar o Dashboard React

**Em outro terminal**, execute:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
npm run dev
```

Você deve ver:

```
  VITE v7.2.4  ready in 500 ms

  ➜  Local:   http://localhost:5173/
```

### Passo 4: Abrir o Dashboard no Navegador

Abra seu navegador e acesse:

```
http://localhost:5173
```

## ✅ Verificação

### No Dashboard:

1. **Status de Conexão**: No canto superior direito deve aparecer **"Conectado"** (verde)
2. **Dados em Tempo Real**: Os cards devem mostrar valores atualizados:
   - Temperatura (ex: 26.70°C)
   - Umidade (ex: 57.00%)
   - Luminosidade (ex: 66.00%)
   - Ocupação (OCUPADA/VAZIA)
   - Número de Pessoas (ex: 4 pessoas)
3. **Gráficos**: Os gráficos de histórico devem mostrar linhas com dados
4. **Alertas**: Se valores estiverem fora do ideal, alertas aparecerão

### No Console do Navegador (F12):

Você deve ver:

```
✅ Conectado ao MQTT
📥 Subscreveu em sala/temperatura
📥 Subscreveu em sala/umidade
...
```

### No Terminal do Proxy Server:

Você deve ver mensagens quando dados chegam:

```
📥 dashboard_... subscreveu em sala/temperatura
```

## 🐛 Troubleshooting

### Dashboard mostra "Desconectado"

**Solução:**
1. Verifique se o proxy server está rodando (Passo 2)
2. Verifique se o proxy server conseguiu conectar ao broker MQTT
3. Abra o console do navegador (F12) e veja se há erros
4. Verifique se a porta 8080 não está sendo usada por outro processo

### Dados não aparecem no dashboard

**Solução:**
1. Verifique se o ESP32 está conectado e publicando dados
2. Verifique o monitor serial do ESP32 - deve mostrar dados sendo publicados
3. Verifique o terminal do proxy server - deve mostrar conexão estabelecida
4. Abra o console do navegador (F12) e veja se há mensagens MQTT chegando

### Erro "Cannot connect to WebSocket"

**Solução:**
1. Certifique-se de que o proxy server está rodando na porta 8080
2. Verifique se não há firewall bloqueando a porta 8080
3. Tente reiniciar o proxy server

### Proxy server não conecta ao broker MQTT

**Solução:**
1. Verifique se o broker MQTT está rodando em `172.26.70.17:1883`
2. Teste a conexão: `ping 172.26.70.17`
3. Verifique se o IP do broker está correto no arquivo `mqtt-proxy-server.js`

## 📊 Estrutura de Execução

```
Terminal 1: Proxy Server MQTT
├── node mqtt-proxy-server.js
└── Conecta: Broker TCP (172.26.70.17:1883) ↔ WebSocket (localhost:8080)

Terminal 2: Dashboard React
├── npm run dev
└── Conecta: WebSocket (localhost:8080) ↔ Navegador (localhost:5173)

ESP32
└── Publica dados → Broker MQTT (172.26.70.17:1883)
```

## 🎯 Fluxo de Dados

```
ESP32 → Broker MQTT (TCP:1883) → Proxy Server → WebSocket (8080) → Dashboard (Navegador)
```

## 📝 Comandos Rápidos

```bash
# Terminal 1 - Proxy Server
cd dashboard
node mqtt-proxy-server.js

# Terminal 2 - Dashboard
cd dashboard
npm run dev

# Acessar no navegador
http://localhost:5173
```

## ⚠️ Importante

- **Mantenha ambos os terminais abertos** enquanto usar o dashboard
- Se fechar o proxy server, o dashboard perderá a conexão
- Se fechar o dashboard (npm run dev), apenas reinicie com `npm run dev`

## 🎉 Pronto!

Se tudo estiver funcionando, você verá:

- ✅ Status "Conectado" no dashboard
- ✅ Dados atualizando em tempo real
- ✅ Gráficos mostrando histórico
- ✅ Alertas quando valores estão fora do ideal
- ✅ Contagem de pessoas atualizando

---

**💡 Dica:** Para ver logs detalhados, abra o console do navegador (F12) e o terminal do proxy server.

