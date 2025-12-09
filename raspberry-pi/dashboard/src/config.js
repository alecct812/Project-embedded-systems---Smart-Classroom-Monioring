// ========== CONFIGURAÇÃO DO DASHBOARD ==========

// Configure o IP do seu broker MQTT aqui
// Conectando diretamente ao Mosquitto via WebSocket
export const MQTT_CONFIG = {
  // URL do Mosquitto WebSocket (porta 9001)
  brokerUrl: 'ws://localhost:9001',
  
  // Tópicos MQTT que o dashboard vai escutar
  topics: {
    temperatura: 'sala/temperatura',
    umidade: 'sala/umidade',
    luminosidade: 'sala/luminosidade',
    presenca: 'sala/presenca',
    ocupacao: 'sala/ocupacao',
    alertas: 'sala/alertas',
    sugestaoAC: 'sala/sugestao_ac'
  },
  
  // Opções de conexão
  options: {
    keepalive: 60,
    clean: true,
    reconnectPeriod: 1000
  }
};

// Faixas ideais (devem corresponder ao ESP32)
export const IDEAL_RANGES = {
  temperatura: {
    min: 20,
    max: 25
  },
  umidade: {
    min: 40,
    max: 60
  },
  luminosidade: {
    min: 30
  }
};

// Configuração dos gráficos
export const CHART_CONFIG = {
  maxDataPoints: 20,  // Número máximo de pontos no gráfico
  refreshInterval: 100 // Intervalo de atualização (ms)
};

