import { useState, useEffect } from 'react';
import mqtt from 'mqtt';

// Hook customizado para gerenciar dados MQTT
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
  const [pessoas, setPessoas] = useState(0);  // Número de pessoas na sala

  // Histórico para gráficos (últimos 20 pontos)
  const [tempHistory, setTempHistory] = useState([]);
  const [humidHistory, setHumidHistory] = useState([]);

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
    console.log(`🔌 Conectando ao broker: ${brokerUrl}`);
    
    // Conecta ao broker MQTT via WebSocket
    const client = mqtt.connect(brokerUrl, {
      reconnectPeriod: 5000,
      connectTimeout: 10000,
    });

    client.on('connect', () => {
      console.log('✅ Conectado ao broker MQTT');
      setIsConnected(true);

      // Subscreve aos tópicos
      const topics = [
        'sala/temperatura',
        'sala/umidade',
        'sala/luminosidade',
        'sala/presenca',
        'sala/ocupacao',
        'sala/alertas',
        'sala/sugestao_ac',
        'sala/pessoas',
        'sala/entrada',
        'sala/saida',
        'sala/status',
      ];

      topics.forEach(topic => {
        client.subscribe(topic, (err) => {
          if (err) {
            console.error(`❌ Erro ao subscrever ${topic}:`, err);
          } else {
            console.log(`📡 Subscrito ao tópico: ${topic}`);
          }
        });
      });
    });

    client.on('message', (topic, message) => {
      const value = message.toString().trim();
      const now = getTimeString();
      setLastUpdate(now);

      console.log(`📨 Mensagem recebida: ${topic} = ${value}`);

      // Processa mensagens baseado no tópico
      switch (topic) {
        case 'sala/temperatura':
          const temp = parseFloat(value);
          if (!isNaN(temp)) {
            setTemperatura(temp);
            addToHistory(setTempHistory, temp);
          }
          break;

        case 'sala/umidade':
          const umid = parseFloat(value);
          if (!isNaN(umid)) {
            setUmidade(umid);
            addToHistory(setHumidHistory, umid);
          }
          break;

        case 'sala/luminosidade':
          const luz = parseFloat(value);
          if (!isNaN(luz)) {
            setLuminosidade(luz);
          }
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
          const numPessoas = parseInt(value, 10);
          if (!isNaN(numPessoas)) {
            setPessoas(numPessoas);
            console.log(`👥 Pessoas na sala: ${numPessoas}`);
          }
          break;

        case 'sala/entrada':
          console.log('🚪➡️ Entrada detectada');
          break;

        case 'sala/saida':
          console.log('🚪⬅️ Saída detectada');
          break;

        case 'sala/status':
          console.log(`📡 Status ESP32: ${value}`);
          break;

        default:
          break;
      }
    });

    client.on('error', (err) => {
      console.error('❌ Erro MQTT:', err);
      setIsConnected(false);
    });

    client.on('close', () => {
      console.log('🔌 Conexão MQTT fechada');
      setIsConnected(false);
    });

    client.on('reconnect', () => {
      console.log('🔄 Tentando reconectar ao MQTT...');
    });

    client.on('offline', () => {
      console.log('⚠️ MQTT offline');
      setIsConnected(false);
    });

    // Cleanup ao desmontar
    return () => {
      if (client) {
        client.end();
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
    humidHistory,
    pessoas  // Número de pessoas na sala
  };
};

export default useMqttData;
