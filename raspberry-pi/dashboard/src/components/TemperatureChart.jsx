import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, ReferenceLine } from 'recharts';

// Gráfico de temperatura em tempo real
const TemperatureChart = ({ data }) => {
  return (
    <div className="bg-white rounded-xl shadow-lg p-6">
      <h3 className="text-xl font-semibold text-gray-800 mb-4">
        📈 Histórico de Temperatura
      </h3>
      <ResponsiveContainer width="100%" height={300}>
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis 
            dataKey="time" 
            tick={{ fontSize: 12 }}
          />
          <YAxis 
            domain={[15, 35]}
            tick={{ fontSize: 12 }}
          />
          <Tooltip />
          <Legend />
          {/* Linhas de referência para faixa ideal */}
          <ReferenceLine y={20} stroke="#82ca9d" strokeDasharray="3 3" label="Min" />
          <ReferenceLine y={25} stroke="#82ca9d" strokeDasharray="3 3" label="Max" />
          {/* Linha de dados */}
          <Line 
            type="monotone" 
            dataKey="value" 
            stroke="#8884d8" 
            strokeWidth={2}
            dot={{ r: 4 }}
            name="Temperatura (°C)"
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default TemperatureChart;

