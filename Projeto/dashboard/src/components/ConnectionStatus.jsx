import React from 'react';

// Indicador de conexão MQTT
const ConnectionStatus = ({ isConnected, lastUpdate }) => {
  return (
    <div className={`flex items-center space-x-3 px-4 py-2 rounded-full ${
      isConnected ? 'bg-green-500' : 'bg-red-500'
    }`}>
      <div className={`w-3 h-3 rounded-full ${
        isConnected ? 'bg-white animate-pulse' : 'bg-gray-300'
      }`} />
      <span className="text-white font-medium">
        {isConnected ? 'Conectado' : 'Desconectado'}
      </span>
      {isConnected && lastUpdate && (
        <span className="text-white text-sm opacity-80">
          | Última atualização: {lastUpdate}
        </span>
      )}
    </div>
  );
};

export default ConnectionStatus;

