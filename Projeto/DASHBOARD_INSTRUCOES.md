# 📊 Instruções - Dashboard Smart Classroom

## 🎯 O que foi criado

Criei um **dashboard web completo e moderno** para monitorar seu sistema em tempo real!

### ✨ Características

- 🌐 **Interface Web Bonita** - Design moderno com gradiente roxo
- 📊 **Gráficos em Tempo Real** - Temperatura e umidade com histórico
- 🎨 **Cards Coloridos** - Verde (normal), Amarelo (aviso), Vermelho (alerta)
- ⚡ **Atualização Instantânea** - Via WebSocket MQTT
- 💡 **Sugestões Inteligentes** - Ligar/desligar ar-condicionado
- ⚠️ **Sistema de Alertas** - Avisos destacados
- 📱 **Responsivo** - Funciona em desktop, tablet e celular

---

## 🚀 COMO RODAR (Passo a Passo)

### ✅ Passo 1: Entrar na pasta

```bash
cd Projeto/dashboard
```

### ✅ Passo 2: Instalar dependências

```bash
npm install
```

**Aguarde a instalação** (pode levar 1-2 minutos na primeira vez).

### ✅ Passo 3: Configurar o IP do Broker

Abra o arquivo: `src/config.js`

Altere a linha 10 para o IP do seu broker MQTT:

```javascript
brokerUrl: 'ws://172.26.70.17:9001',  // <-- Coloque seu IP aqui
```

**IMPORTANTE**: 
- Use `ws://` (WebSocket)
- Porta deve ser `9001` (WebSocket)
- O IP deve ser o mesmo do broker MQTT

### ✅ Passo 4: Configurar Mosquitto com WebSocket

O dashboard precisa de WebSocket (porta 9001).

**No seu Mac:**

```bash
# Editar configuração do Mosquitto
nano /opt/homebrew/etc/mosquitto/mosquitto.conf
```

Adicione estas linhas:

```conf
listener 1883
listener 9001
protocol websockets
allow_anonymous true
```

Salve e reinicie:

```bash
brew services restart mosquitto
```

### ✅ Passo 5: Rodar o Dashboard

```bash
npm run dev
```

Você verá:

```
  VITE v5.0.8  ready in 500 ms

  ➜  Local:   http://localhost:3000/
  ➜  Network: http://192.168.x.x:3000/
```

### ✅ Passo 6: Abrir no Navegador

Abra: http://localhost:3000

---

## 🎨 O que você verá

### Cards Principais
- 🌡️ **Temperatura** - Com sugestão de AC
- 💧 **Umidade** - Percentual
- 💡 **Luminosidade** - Com sugestão de luzes
- 🚪 **Ocupação** - Status da sala

### Card de Presença
- 👤 **Detecção** - Mostra se tem alguém na sala

### Painel de Alertas
- ⚠️ **Alertas Ativos** - Lista de avisos
- ✅ **Status OK** - Quando tudo está normal

### Gráficos
- 📈 **Temperatura** - Últimos 20 pontos
- 📈 **Umidade** - Últimos 20 pontos

---

## 🔍 Testando a Conexão

### Teste 1: Broker está funcionando?

```bash
mosquitto_sub -h localhost -t "sala/#" -v
```

Você deve ver as mensagens do ESP32.

### Teste 2: WebSocket está funcionando?

```bash
telnet localhost 9001
```

Deve conectar sem erro.

### Teste 3: Dashboard conectou?

No navegador, abra o **Console** (F12) e procure:

```
✅ Conectado ao broker MQTT
📡 Subscrito ao tópico: sala/temperatura
📡 Subscrito ao tópico: sala/umidade
...
```

---

## 🐛 Resolvendo Problemas

### ❌ Problema: "Cannot connect"

**Causa**: Broker não está rodando ou WebSocket não configurado

**Solução**:
```bash
# Verificar se mosquitto está rodando
brew services list

# Reiniciar
brew services restart mosquitto

# Testar
telnet localhost 9001
```

### ❌ Problema: Dashboard abre mas sem dados

**Causa**: ESP32 não está publicando ou IP errado

**Solução**:
1. Verifique o monitor serial do ESP32
2. Confirme que o ESP32 está conectado
3. Verifique o IP em `src/config.js`
4. Use `mosquitto_sub` para ver se mensagens chegam

### ❌ Problema: Gráficos não aparecem

**Causa**: Faltam dados no histórico

**Solução**:
- Aguarde 10-20 segundos
- Os gráficos precisam de alguns pontos de dados
- Recarregue a página (F5)

---

## 🌐 Acessar de Outro Computador/Celular

### Passo 1: Descobrir seu IP

```bash
ifconfig | grep "inet "
# Exemplo de saída: inet 192.168.1.100
```

### Passo 2: Acessar

No celular/outro PC, abra:
```
http://192.168.1.100:3000
```

**Importante**: Todos os dispositivos devem estar na mesma rede WiFi.

---

## 📦 Estrutura do Projeto

```
dashboard/
├── src/
│   ├── components/          # Componentes visuais
│   │   ├── MetricCard.jsx   # Card de temperatura/umidade/luz
│   │   ├── StatusCard.jsx   # Card de status (presença/ocupação)
│   │   ├── AlertPanel.jsx   # Painel de alertas
│   │   ├── TemperatureChart.jsx
│   │   ├── HumidityChart.jsx
│   │   └── ConnectionStatus.jsx
│   ├── hooks/
│   │   └── useMqttData.js   # Gerencia conexão MQTT
│   ├── config.js            # ⭐ CONFIGURE AQUI
│   ├── App.jsx              # Aplicação principal
│   └── main.jsx
├── package.json
├── README.md                # Documentação completa
└── GUIA_RAPIDO.md          # Guia rápido
```

---

## 🎯 Checklist Rápido

- [ ] Mosquitto rodando com WebSocket (porta 9001)
- [ ] ESP32 conectado e publicando dados
- [ ] IP configurado em `src/config.js`
- [ ] `npm install` executado
- [ ] `npm run dev` rodando
- [ ] Dashboard aberto em http://localhost:3000
- [ ] Indicador de conexão está VERDE
- [ ] Dados aparecem nos cards
- [ ] Gráficos mostram histórico

---

## 💻 Comandos Úteis

```bash
# Iniciar dashboard
npm run dev

# Build para produção
npm run build

# Preview do build
npm run preview

# Instalar dependências
npm install

# Ver logs do mosquitto
tail -f /opt/homebrew/var/log/mosquitto/mosquitto.log
```

---

## 📊 Dados Monitorados

| Métrica | Faixa Ideal | Ação |
|---------|-------------|------|
| Temperatura | 20-25°C | Sugestão de AC |
| Umidade | 40-60% | Alerta visual |
| Luminosidade | >30% | Sugestão de luzes |
| Presença | - | LED no ESP32 |
| Ocupação | - | Status visual |

---

## 🎨 Tecnologias Usadas

- **React 18** - Framework moderno
- **Vite** - Build super rápido
- **TailwindCSS** - Estilização linda
- **MQTT.js** - Comunicação em tempo real
- **Recharts** - Gráficos interativos

---

## 🏆 Pronto!

Seu dashboard está funcionando! 

**Próximos passos:**
1. Teste com o ESP32 rodando
2. Veja os dados aparecendo em tempo real
3. Observe os alertas quando temperatura sair do ideal
4. Confira as sugestões inteligentes

**Dúvidas?** Veja os arquivos:
- `README.md` - Documentação completa
- `GUIA_RAPIDO.md` - Referência rápida

---

**Desenvolvido com ❤️ para o Grupo 10 - CESAR School 2025**

