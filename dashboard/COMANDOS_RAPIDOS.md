# 🚀 Comandos Rápidos para Executar o Dashboard

## 📍 Localização do Projeto

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
```

## ⚡ Comandos Essenciais

### 1️⃣ Instalar dependências (primeira vez apenas)
```bash
npm install
```

### 2️⃣ Iniciar o servidor de desenvolvimento
```bash
npm run dev
```

### 3️⃣ Abrir no navegador
Acesse: **http://localhost:5173**

---

## 🔧 Se Precisar do Proxy Server

Se o broker MQTT não tiver WebSocket, execute em **outro terminal**:

```bash
cd /Users/Pedro/DOC/Project-embedded-systems---Smart-Classroom-Monioring/dashboard
node mqtt-proxy-server.js
```

Depois edite `src/config.js` e mude para:
```javascript
wsUrl: 'ws://localhost:8080',
```

---

## ✅ Checklist Rápido

- [ ] Node.js instalado? (`node --version`)
- [ ] Broker MQTT rodando?
- [ ] ESP32 conectado e publicando dados?
- [ ] Executei `npm install`?
- [ ] Executei `npm run dev`?
- [ ] Abri `http://localhost:5173` no navegador?

---

## 🐛 Problemas?

1. **Erro de conexão MQTT?**
   - Verifique o IP no `src/config.js`
   - Use o proxy server se necessário

2. **Dados não aparecem?**
   - Verifique console do navegador (F12)
   - Confirme que ESP32 está publicando

3. **Porta em uso?**
   - Feche outros processos na porta 5173
   - Ou use: `npm run dev -- --port 3000`

---

**🎉 Pronto!** Seu dashboard deve estar funcionando!

