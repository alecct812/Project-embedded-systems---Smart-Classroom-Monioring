# Sistema de Monitoramento de Sala de Aula Inteligente

## 📋 Descrição do Projeto

Sistema IoT usando ESP32 para monitorar condições ambientais e ocupação de uma sala de aula, com integração MQTT e Node-RED para dashboard em tempo real.

**Diferencial:** Usa FreeRTOS com filas e tasks para processamento paralelo e eficiente, com detecção automática de sensores!

## 🔧 Componentes Necessários

Você pode conectar TODOS os sensores em UM único ESP32, ou distribuir entre vários:

- ESP32 (1 ou mais)
- Sensor DHT22 (temperatura e umidade)
-  LDR (sensores de luminosidade)
- 2x Resistor 1kΩ (para os LDRs)
- 2x Sensor TCRT-5000 (movimento/presença)

## 🔌 Esquema de Ligações

### Pinos Configurados:
- **DHT11 DATA** → GPIO 4
- **LDR 1** → GPIO 32 (com resistor 1kΩ ao GND)
- **TCRT-5000 1** -> gpio 13
- **TCRT-5000 1** -> gpio 12


## 👥 Grupo 10

Projeto desenvolvido como parte da disciplina de Sistemas Embarcados.
