import React from 'react';
import useMqttData from './hooks/useMqttData';
import MetricCard from './components/MetricCard';
import StatusCard from './components/StatusCard';
import AlertPanel from './components/AlertPanel';
import TemperatureChart from './components/TemperatureChart';
import HumidityChart from './components/HumidityChart';
import ConnectionStatus from './components/ConnectionStatus';
import { MQTT_CONFIG } from './config';

// URL do broker MQTT vem do arquivo de configuração
const BROKER_URL = MQTT_CONFIG.brokerUrl;

function App() {
  // Hook customizado que gerencia todos os dados MQTT
  const {
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
  } = useMqttData(BROKER_URL);

  // Determina status da temperatura
  const getTempStatus = () => {
    if (!temperatura) return 'normal';
    if (temperatura < 20 || temperatura > 25) return 'danger';
    if (temperatura < 21 || temperatura > 24) return 'warning';
    return 'normal';
  };

  // Determina status da umidade
  const getHumidStatus = () => {
    if (!umidade) return 'normal';
    if (umidade < 40 || umidade > 60) return 'danger';
    if (umidade < 45 || umidade > 55) return 'warning';
    return 'normal';
  };

  // Determina status da luminosidade
  const getLuxStatus = () => {
    if (!luminosidade) return 'normal';
    if (luminosidade < 30) return 'warning';
    return 'normal';
  };

  return (
    <div className="min-h-screen py-8 px-4">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <div>
            <h1 className="text-4xl font-bold text-white mb-2">
              🏫 Smart Classroom Monitor
            </h1>
            <p className="text-white text-opacity-90">
              Sistema de Monitoramento Inteligente - Grupo 10
            </p>
          </div>
          <ConnectionStatus isConnected={isConnected} lastUpdate={lastUpdate} />
        </div>

        {/* Grid de Cards Principais */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8">
          <MetricCard
            title="Temperatura"
            value={temperatura?.toFixed(1)}
            unit="°C"
            icon="🌡️"
            status={getTempStatus()}
            suggestion={sugestaoAC ? `${sugestaoAC} ar-condicionado` : null}
          />

          <MetricCard
            title="Umidade"
            value={umidade?.toFixed(1)}
            unit="%"
            icon="💧"
            status={getHumidStatus()}
          />

          <MetricCard
            title="Luminosidade"
            value={luminosidade?.toFixed(1)}
            unit="%"
            icon="💡"
            status={getLuxStatus()}
            suggestion={luminosidade && luminosidade < 30 ? 'Considerar acender luzes' : null}
          />

          <StatusCard
            title="Ocupação"
            value={ocupacao || 'Aguardando...'}
            icon="🚪"
            isActive={ocupacao === 'OCUPADA'}
          />
        </div>

        {/* Status de Presença */}
        <div className="mb-8">
          <StatusCard
            title="Detecção de Presença"
            value={presenca === 'DETECTADA' ? 'Presença Detectada' : 'Sem Presença'}
            icon="👤"
            isActive={presenca === 'DETECTADA'}
          />
        </div>

        {/* Painel de Alertas */}
        <div className="mb-8">
          <AlertPanel alerts={alertas} />
        </div>

        {/* Gráficos */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <TemperatureChart data={tempHistory} />
          <HumidityChart data={humidHistory} />
        </div>

        {/* Footer */}
        <div className="mt-8 text-center text-white text-opacity-80">
          <p className="text-sm">
            Desenvolvido com React + MQTT + ESP32 | CESAR School 2025
          </p>
        </div>
      </div>
    </div>
  );
}

export default App;

