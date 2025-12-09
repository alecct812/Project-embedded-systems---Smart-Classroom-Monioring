/**
 * Servidor Proxy MQTT para WebSocket
 * 
 * Este servidor faz a ponte entre o navegador (WebSocket) e o broker MQTT (TCP)
 * Funciona como um proxy simples que repassa mensagens MQTT
 * 
 * Como usar:
 * 1. Certifique-se de ter instalado: npm install ws mqtt
 * 2. node mqtt-proxy-server.js
 * 3. O dashboard conectará automaticamente em ws://localhost:8080
 */

import { WebSocketServer, WebSocket } from 'ws';
import mqtt from 'mqtt';

// Configuração
const MQTT_BROKER = `mqtt://172.26.70.17:1883`;
const WS_PORT = 8080;

// Cria servidor WebSocket
const wss = new WebSocketServer({ port: WS_PORT });

console.log(`🚀 Servidor Proxy MQTT iniciado na porta ${WS_PORT}`);
console.log(`📡 Broker MQTT: ${MQTT_BROKER}`);
console.log(`🌐 Dashboard deve usar: ws://localhost:${WS_PORT}\n`);

wss.on('connection', (ws) => {
  const clientId = `dashboard_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
  console.log(`✅ Cliente WebSocket conectado: ${clientId}`);

  // Conecta ao broker MQTT TCP
  const mqttClient = mqtt.connect(MQTT_BROKER, {
    clientId: clientId,
    reconnectPeriod: 5000,
    connectTimeout: 10000,
  });

  // Quando conecta ao broker MQTT
  mqttClient.on('connect', () => {
    console.log(`✅ ${clientId} conectado ao broker MQTT`);
    
    // Subscreve aos tópicos publicados pelo ESP32
    const topics = [
      'sala/temperatura',
      'sala/umidade',
      'sala/luminosidade',
      'sala/ocupacao',
      'sala/alertas',
      'sala/pessoas',
      'sala/entrada',
      'sala/saida',
      'sala/status',
    ];

    topics.forEach((topic) => {
      mqttClient.subscribe(topic, { qos: 0 }, (err) => {
        if (err) {
          console.error(`❌ Erro ao subscrever ${topic}:`, err);
        } else {
          console.log(`📥 ${clientId} subscreveu em ${topic}`);
        }
      });
    });
  });

  // Repassa mensagens MQTT para o WebSocket
  // O mqtt.js precisa receber mensagens no formato correto
  mqttClient.on('message', (topic, message, packet) => {
    if (ws.readyState === WebSocket.OPEN) {
      try {
        // Envia mensagem MQTT completa incluindo tópico
        // Formato: tópico + payload separados por quebra de linha
        const payload = message.toString();
        ws.send(JSON.stringify({
          topic: topic,
          payload: payload,
          timestamp: Date.now()
        }));
        console.log(`📤 ${clientId} → ${topic}: ${payload}`);
      } catch (error) {
        console.error(`❌ Erro ao enviar mensagem para ${clientId}:`, error.message);
      }
    }
  });

  mqttClient.on('error', (error) => {
    console.error(`❌ Erro MQTT para ${clientId}:`, error.message);
  });

  mqttClient.on('offline', () => {
    console.log(`⚠️ ${clientId} offline`);
  });

  mqttClient.on('reconnect', () => {
    console.log(`🔄 ${clientId} reconectando...`);
  });

  // Quando recebe mensagem do WebSocket (do navegador via mqtt.js)
  ws.on('message', (data) => {
    // Por enquanto apenas escutamos (subscribe)
    // Se necessário, pode implementar publicação também
  });

  // Quando o WebSocket fecha
  ws.on('close', () => {
    console.log(`❌ Cliente WebSocket desconectado: ${clientId}`);
    if (mqttClient) {
      mqttClient.end();
    }
  });

  ws.on('error', (error) => {
    console.error(`❌ Erro WebSocket para ${clientId}:`, error.message);
    if (mqttClient) {
      mqttClient.end();
    }
  });
});

// Graceful shutdown
process.on('SIGINT', () => {
  console.log('\n🛑 Encerrando servidor proxy...');
  wss.close(() => {
    console.log('✅ Servidor encerrado');
    process.exit(0);
  });
});
