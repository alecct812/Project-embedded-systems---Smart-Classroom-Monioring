import React from 'react';

// Card para status de ocupação e presença
const StatusCard = ({ title, value, icon, isActive }) => {
  return (
    <div className={`rounded-xl shadow-lg p-6 border-2 transition-all ${
      isActive 
        ? 'bg-blue-100 border-blue-400 text-blue-900' 
        : 'bg-gray-100 border-gray-300 text-gray-600'
    }`}>
      {/* Header do card */}
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold">{title}</h3>
        <span className={`text-3xl ${isActive ? 'animate-pulse-soft' : ''}`}>
          {icon}
        </span>
      </div>

      {/* Status */}
      <div className="text-2xl font-bold">
        {value || 'Aguardando...'}
      </div>

      {/* Indicador visual */}
      <div className="mt-4">
        <div className={`h-2 rounded-full ${
          isActive ? 'bg-blue-500' : 'bg-gray-300'
        }`} />
      </div>
    </div>
  );
};

export default StatusCard;

