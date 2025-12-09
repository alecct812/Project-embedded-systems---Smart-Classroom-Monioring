import React from 'react';

// Painel de alertas do sistema
const AlertPanel = ({ alerts }) => {
  if (!alerts || alerts.length === 0) {
    return (
      <div className="bg-green-100 border-2 border-green-300 rounded-xl p-6 shadow-lg">
        <div className="flex items-center">
          <span className="text-3xl mr-3">✅</span>
          <div>
            <h3 className="text-lg font-semibold text-green-800">
              Sistema Operando Normalmente
            </h3>
            <p className="text-green-600">Todos os parâmetros estão dentro do ideal</p>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="bg-orange-100 border-2 border-orange-300 rounded-xl p-6 shadow-lg">
      <div className="flex items-start">
        <span className="text-3xl mr-3 animate-pulse-soft">⚠️</span>
        <div className="flex-1">
          <h3 className="text-lg font-semibold text-orange-800 mb-3">
            Alertas Ativos ({alerts.length})
          </h3>
          <div className="space-y-2">
            {alerts.slice(0, 5).map((alert, index) => (
              <div 
                key={index} 
                className="bg-white bg-opacity-60 rounded-lg p-3 text-orange-900"
              >
                <p className="font-medium">{alert}</p>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};

export default AlertPanel;

