# 🔧 Modo Teste de Hardware

## 📝 Como Ativar o Modo Teste

### **Opção 1: Descomentar a linha (RECOMENDADO)**

1. Abra o arquivo `src/main.cpp`
2. Procure pela linha **54**:
   ```cpp
   // #define MODO_TESTE_HARDWARE
   ```

3. **Remova as duas barras** `//` para descomentar:
   ```cpp
   #define MODO_TESTE_HARDWARE
   ```

4. **Salve o arquivo** (Ctrl+S)
5. **Upload** para o ESP32 (Ctrl+Alt+U)
6. **Abra o Monitor Serial** (Ctrl+Alt+S)

### **Opção 2: Adicionar temporariamente**

Adicione esta linha logo após os `#include`:
```cpp
#define MODO_TESTE_HARDWARE
```

---

## 📊 O Que o Teste Faz

O modo teste executa **continuamente** (a cada 10 segundos) os seguintes testes:

### **1️⃣ Teste DHT11 (Temperatura e Umidade)**
- ✅ Verifica se o sensor responde
- 🌡️ Mostra temperatura em °C
- 💧 Mostra umidade em %
- ⚠️ Alerta se valores fora do normal

### **2️⃣ Teste LDR (Luminosidade)**
- ✅ Lê o valor ADC (0-4095)
- 💡 Converte para % de luminosidade
- 📝 Sugere teste: cobrir com a mão
- ⚠️ Detecta curtos ou problemas de conexão

### **3️⃣ Teste PIR (Sensor de Presença)**
- ✅ Lê estado do sensor
- 👤 Indica se há movimento detectado
- 📝 Sugere teste: mover a mão na frente
- ℹ️ Lembra que PIR precisa calibrar (~60s)

### **4️⃣ Teste LED**
- ✅ Pisca LED 3x
- 💡 Mostra quando liga/desliga
- ✔️ Confirma funcionamento

### **5️⃣ Teste Integração PIR + LED**
- ✅ LED segue estado do PIR por 5 segundos
- 🔄 Simula funcionamento real do sistema

---

## 🖥️ Exemplo de Saída no Monitor Serial

```
╔══════════════════════════════════════════════════╗
║       MODO TESTE DE HARDWARE ATIVADO           ║
║   Testando componentes continuamente...         ║
╚══════════════════════════════════════════════════╝

Configuração dos Pinos:
  DHT11 → GPIO 15
  LDR   → GPIO 34
  PIR   → GPIO 13
  LED   → GPIO 32

════════════════════════════════════════════════════

════════════════════════════════════════════════════
              TESTE #1
════════════════════════════════════════════════════

[1] Testando DHT11 (GPIO 15)...
    ✅ DHT11 FUNCIONANDO
    🌡️  Temperatura: 24.50 °C
    💧 Umidade: 52.30 %

[2] Testando LDR (GPIO 34)...
    ✅ LDR FUNCIONANDO
    💡 Valor ADC: 2048 (0-4095)
    💡 Luminosidade: 50.00 %
    → Teste: Cubra o LDR com a mão (valor deve diminuir)

[3] Testando PIR (GPIO 13)...
    ✅ PIR FUNCIONANDO
    🚶 Status: SEM MOVIMENTO
    → Teste: Mexa a mão na frente do sensor
    → Nota: PIR precisa de ~60s para calibrar após ligar

[4] Testando LED (GPIO 32)...
    💡 Piscando LED 3x...
    ● LED LIGADO  ○ LED APAGADO
    ● LED LIGADO  ○ LED APAGADO
    ● LED LIGADO  ○ LED APAGADO
    ✅ LED FUNCIONANDO

[5] Testando integração PIR + LED...
    LED segue estado do PIR por 5 segundos...
    🚶 🚶 🚶 🚶 🚶 🚶 🚶 🚶 🚶 🚶 
    ✅ Integração OK

════════════════════════════════════════════════════
                  RESUMO DO TESTE
════════════════════════════════════════════════════
  DHT11: ✅ OK
  LDR:   ✅ OK
  PIR:   ✅ OK (sempre funcional se conectado)
  LED:   ✅ OK

════════════════════════════════════════════════════

Próximo teste em 10 segundos...
Pressione RESET para reiniciar o ESP32
```

---

## ❌ Exemplo de Saída com Erro (DHT11 Desconectado)

```
[1] Testando DHT11 (GPIO 15)...
    ❌ DHT11 NÃO RESPONDE
    → Verifique:
      - VCC → 3.3V
      - DATA → GPIO 15
      - GND → GND
      - Resistor pull-up 10kΩ (opcional mas recomendado)
```

---

## 🔄 Como Desativar o Modo Teste

1. Abra `src/main.cpp`
2. Comente novamente a linha:
   ```cpp
   // #define MODO_TESTE_HARDWARE
   ```
3. Salve (Ctrl+S)
4. Faça upload novamente (Ctrl+Alt+U)
5. O sistema voltará ao modo normal (MQTT + Tasks)

---

## 🎯 Quando Usar o Modo Teste

✅ **Use o modo teste quando:**
- Montar o circuito pela primeira vez
- Trocar componentes
- Suspeitar que algum sensor está queimado
- Debugar problemas de hardware
- Verificar conexões físicas
- Demonstrar funcionamento dos sensores

❌ **NÃO use o modo teste quando:**
- Quiser testar a comunicação MQTT
- Quiser ver o dashboard Node-RED
- Sistema estiver em produção

---

## 💡 Dicas

1. **Baud Rate:** O modo teste usa **115200 bps** (igual ao sistema normal)

2. **Tempo entre testes:** 10 segundos (configurável na função)

3. **Loop infinito:** O modo teste nunca sai. Para voltar ao normal, desative e faça upload novamente.

4. **Monitor Serial:** Mantenha aberto para ver os resultados em tempo real

5. **LED piscando:** Se o LED piscar 3x a cada teste, significa que está funcionando corretamente

---

## 🐛 Troubleshooting

### DHT11 não responde:
- Verifique alimentação (3.3V)
- Confirme GPIO 15
- Aguarde 2 segundos após ligar
- Considere adicionar resistor pull-up 10kΩ

### LDR sempre 0 ou 4095:
- Verifique divisor de tensão
- Confirme resistor de 10kΩ
- GPIO 34 deve estar livre

### PIR não detecta:
- Aguarde 60 segundos após ligar (calibração)
- Verifique alimentação 5V
- Ajuste os potenciômetros do sensor

### LED não pisca:
- Verifique polaridade (ânodo no GPIO 32)
- Confirme resistor 220Ω
- Teste com outro LED

---

**Criado para facilitar debug e validação de hardware! 🚀**
