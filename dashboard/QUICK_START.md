# 🚀 Guia Rápido de Início

## Opção 1: Broker MQTT com WebSocket Nativo (Recomendado)

Se o seu broker MQTT já tem suporte a WebSocket:

1. **Configure o IP do broker** no arquivo `src/config.js`:
```javascript
wsUrl: 'ws://172.26.70.17:9001', // Ajuste o IP e porta conforme necessário
```

2. **Instale dependências e inicie**:
```bash
cd dashboard
npm install
npm run dev
```

3. **Abra o navegador** em `http://localhost:5173`

## Opção 2: Usar Proxy Server (Se o broker não tiver WebSocket)

Se o seu broker MQTT não tem suporte a WebSocket:

1. **Instale dependências adicionais**:
```bash
cd dashboard
npm install ws mqtt
```

2. **Inicie o proxy server** (em um terminal separado):
```bash
node mqtt-proxy-server.js
```

3. **Configure o dashboard** para usar o proxy no arquivo `src/config.js`:
```javascript
wsUrl: 'ws://localhost:8080', // Porta do proxy
```

4. **Inicie o dashboard** (em outro terminal):
```bash
npm run dev
```

5. **Abra o navegador** em `http://localhost:5173`

## ⚠️ Problemas Comuns

### Dashboard não conecta ao MQTT?

1. **Verifique se o broker está rodando**
2. **Confirme o IP/porta** no arquivo `config.js`
3. **Abra o console do navegador** (F12) para ver erros
4. **Teste a conexão** usando um cliente MQTT (ex: MQTTX)

### Dados não aparecem?

1. **Verifique se o ESP32 está publicando** nos tópicos corretos
2. **Confirme os tópicos** no arquivo `config.js` correspondem aos do ESP32
3. **Verifique a conexão** do ESP32 com o broker MQTT

### Erro de CORS ou WebSocket?

- Certifique-se de que o broker permite conexões WebSocket
- Se usar proxy, verifique se está rodando na porta correta
- Tente usar `wss://` (WebSocket seguro) se disponível

## 📝 Checklist

- [ ] Broker MQTT está rodando
- [ ] IP/porta configurados corretamente em `config.js`
- [ ] ESP32 está conectado e publicando dados
- [ ] Dashboard está rodando (`npm run dev`)
- [ ] Navegador está acessando `http://localhost:5173`
- [ ] Console do navegador não mostra erros

## 🎯 Próximos Passos

Após conectar com sucesso:
- Os dados devem aparecer em tempo real nos cards
- Gráficos devem mostrar histórico
- Alertas aparecerão quando valores estiverem fora do ideal
- Sugestões de ações serão exibidas automaticamente


