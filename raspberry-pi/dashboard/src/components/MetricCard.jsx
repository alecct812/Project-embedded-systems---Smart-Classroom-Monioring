import React from 'react';

// Componente reutilizável para cards de métricas
const MetricCard = ({ title, value, unit, icon, status, suggestion }) => {
  // Define cor do status (normal, warning, danger)
  const statusColors = {
    normal: 'bg-green-100 border-green-300 text-green-800',
    warning: 'bg-yellow-100 border-yellow-300 text-yellow-800',
    danger: 'bg-red-100 border-red-300 text-red-800',
  };

  return (
    <div className={`rounded-xl shadow-lg p-6 border-2 ${statusColors[status] || 'bg-white border-gray-200'}`}>
      {/* Header do card */}
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-semibold text-gray-700">{title}</h3>
        <span className="text-3xl">{icon}</span>
      </div>

      {/* Valor principal */}
      <div className="mb-2">
        <span className="text-4xl font-bold text-gray-800">
          {value !== null ? value : '--'}
        </span>
        <span className="text-xl text-gray-600 ml-2">{unit}</span>
      </div>

      {/* Sugestão (se houver) */}
      {suggestion && (
        <div className="mt-3 p-2 bg-white bg-opacity-50 rounded-lg">
          <p className="text-sm font-medium">💡 {suggestion}</p>
        </div>
      )}
    </div>
  );
};

export default MetricCard;

