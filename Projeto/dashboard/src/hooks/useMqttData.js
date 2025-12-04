import { useState, useEffect, useRef } from 'react';

// Hook customizado para gerenciar dados via WebSocket proxy
const useMqttData = (brokerUrl) => {
  // Estados para todos os dados
  const [isConnected, setIsConnected] = useState(false);
  const [temperatura, setTemperatura] = useState(null);
  const [umidade, setUmidade] = useState(null);
  const [luminosidade, setLuminosidade] = useState(null);
  const [presenca, setPresenca] = useState(null);
  const [ocupacao, setOcupacao] = useState(null);
  const [alertas, setAlertas] = useState([]);
  const [sugestaoAC, setSugestaoAC] = useState(null);
  const [lastUpdate, setLastUpdate] = useState(null);

  // Histórico para gráficos (últimos 20 pontos)
  const [tempHistory, setTempHistory] = useState([]);
  const [humidHistory, setHumidHistory] = useState([]);

  const wsRef = useRef(null);
  const reconnectTimeoutRef = useRef(null);

  // Função auxiliar para formatar hora
  const getTimeString = () => {
    const now = new Date();
    return `${now.getHours().toString().padStart(2, '0')}:${now.getMinutes().toString().padStart(2, '0')}:${now.getSeconds().toString().padStart(2, '0')}`;
  };

  // Função auxiliar para adicionar ao histórico
  const addToHistory = (setHistory, value) => {
    const timeStr = getTimeString();
    setHistory(prev => {
      const newHistory = [...prev, { time: timeStr, value: parseFloat(value) }];
      // Mantém apenas os últimos 20 pontos
      return newHistory.slice(-20);
    });
  };

  useEffect(() => {
    // Conecta ao proxy WebSocket
    const connectWebSocket = () => {
      console.log(`🔌 Conectando ao proxy: ${brokerUrl}`);
      const ws = new WebSocket(brokerUrl);
      wsRef.current = ws;

      ws.onopen = () => {
        console.log('✅ Conectado ao proxy WebSocket');
        setIsConnected(true);
        // O proxy já subscreve automaticamente aos tópicos
      };

      ws.onmessage = (event) => {
        try {
          // O proxy envia mensagens no formato JSON
          const data = JSON.parse(event.data);
          const { topic, payload } = data;
          const value = payload;
          const now = getTimeString();
          setLastUpdate(now);

          console.log(`📨 Recebido: ${topic} = ${value}`);

          // Processa mensagens baseado no tópico
          switch (topic) {
            case 'sala/temperatura':
              const temp = parseFloat(value);
              setTemperatura(temp);
              addToHistory(setTempHistory, temp);
              break;

            case 'sala/umidade':
              const umid = parseFloat(value);
              setUmidade(umid);
              addToHistory(setHumidHistory, umid);
              break;

            case 'sala/luminosidade':
              setLuminosidade(parseFloat(value));
              break;

            case 'sala/presenca':
              setPresenca(value);
              break;

            case 'sala/ocupacao':
              setOcupacao(value);
              break;

            case 'sala/alertas':
              // Adiciona alerta à lista (mantém últimos 10)
              setAlertas(prev => {
                const newAlerts = [value, ...prev].slice(0, 10);
                return newAlerts;
              });
              break;

            case 'sala/sugestao_ac':
              setSugestaoAC(value);
              break;

            case 'sala/pessoas':
              // Tópico de contagem de pessoas
              break;

            case 'sala/status':
              // Status do ESP32
              break;

            default:
              console.log(`Tópico desconhecido: ${topic}`, value);
              break;
          }
        } catch (error) {
          console.error('❌ Erro ao processar mensagem:', error);
        }
      };

      ws.onerror = (error) => {
        console.error('❌ Erro WebSocket:', error);
        setIsConnected(false);
      };

      ws.onclose = () => {
        console.log('🔌 Conexão WebSocket fechada');
        setIsConnected(false);
        
        // Reconecta após 5 segundos
        if (reconnectTimeoutRef.current) {
          clearTimeout(reconnectTimeoutRef.current);
        }
        reconnectTimeoutRef.current = setTimeout(() => {
          console.log('🔄 Tentando reconectar...');
          connectWebSocket();
        }, 5000);
      };
    };

    connectWebSocket();

    // Cleanup ao desmontar
    return () => {
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current);
      }
      if (wsRef.current) {
        wsRef.current.close();
      }
    };
  }, [brokerUrl]);

  return {
    isConnected,
    temperatura,
    umidade,
    luminosidade,
    presenca,
    ocupacao,
    alertas,
    sugestaoAC,
    lastUpdate,
    tempHistory,
    humidHistory
  };
};

export default useMqttData;
