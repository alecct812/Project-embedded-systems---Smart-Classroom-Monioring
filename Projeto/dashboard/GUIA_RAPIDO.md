# 🚀 Guia Rápido - Dashboard

## ⚡ Inicio Rápido (3 minutos)

### 1️⃣ Instalar

```bash
cd dashboard
npm install
```

### 2️⃣ Configurar Broker MQTT

Edite `src/App.jsx` (linha 9):

```javascript
const BROKER_URL = 'ws://172.26.70.17:9001';  // Seu IP aqui
```

### 3️⃣ Rodar

```bash
npm run dev
```

Abra: http://localhost:3000

---

## 🔧 Configurar Broker Mosquitto com WebSocket

### macOS (Homebrew)

```bash
# Instalar
brew install mosquitto

# Criar configuração
cat > /opt/homebrew/etc/mosquitto/mosquitto.conf << EOF
listener 1883
listener 9001
protocol websockets
allow_anonymous true
EOF

# Iniciar
mosquitto -c /opt/homebrew/etc/mosquitto/mosquitto.conf
```

### Linux (Ubuntu/Debian)

```bash
# Instalar
sudo apt update
sudo apt install mosquitto mosquitto-clients

# Configurar
sudo nano /etc/mosquitto/mosquitto.conf
```

Adicione:

```conf
listener 1883
listener 9001
protocol websockets
allow_anonymous true
```

```bash
# Reiniciar
sudo systemctl restart mosquitto
```

### Windows

1. Baixe: https://mosquitto.org/download/
2. Instale
3. Edite `C:\Program Files\mosquitto\mosquitto.conf`
4. Adicione as linhas acima
5. Reinicie o serviço

---

## ✅ Testar Conexão

### Teste 1: Broker está rodando?

```bash
mosquitto_sub -h localhost -t "teste" -v
```

### Teste 2: WebSocket funcionando?

```bash
telnet localhost 9001
```

### Teste 3: ESP32 publicando?

```bash
mosquitto_sub -h localhost -t "sala/#" -v
```

Você deve ver mensagens como:
```
sala/temperatura 26.5
sala/umidade 57.0
```

---

## 📊 Dados do Sistema

O dashboard mostra:

✅ **Temperatura** - Faixa ideal: 20-25°C  
✅ **Umidade** - Faixa ideal: 40-60%  
✅ **Luminosidade** - 0-100%  
✅ **Presença** - DETECTADA / NAO_DETECTADA  
✅ **Ocupação** - OCUPADA / VAZIA  
✅ **Alertas** - Avisos em tempo real  
✅ **Sugestões** - LIGAR/DESLIGAR AC  

---

## 🐛 Problemas Comuns

### ❌ "Cannot connect to broker"

**Solução**: 
1. Verifique se mosquitto está rodando
2. Confirme que a porta 9001 está aberta
3. Teste: `telnet localhost 9001`

### ❌ "Dados não aparecem"

**Solução**:
1. Verifique o ESP32 no monitor serial
2. Confirme que o ESP32 está conectado ao mesmo broker
3. Use `mosquitto_sub` para ver se mensagens chegam

### ❌ "WebSocket error"

**Solução**:
1. Certifique-se que o mosquitto tem `protocol websockets`
2. Reinicie o broker
3. Limpe cache do navegador

---

## 🌐 Acessar de outro dispositivo

1. Descubra seu IP:

```bash
# macOS/Linux
ifconfig | grep "inet "

# Windows
ipconfig
```

2. Acesse: `http://SEU_IP:3000`

---

## 📦 Build para Produção

```bash
npm run build
npm run preview
```

Os arquivos otimizados ficam em `dist/`.

---

## 💡 Dicas

- 🔄 Dashboard atualiza automaticamente em tempo real
- 📈 Gráficos mantém últimos 20 pontos
- ⚠️ Alertas somem após nova leitura normal
- 🎨 Interface responsiva funciona em mobile

---

**Precisa de ajuda?** Veja o `README.md` completo!

