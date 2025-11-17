/*
 * =====================================================================
 * SISTEMA DE MONITORAMENTO DE SALA DE AULA INTELIGENTE
 * =====================================================================
 * Projeto: IoT - Monitoramento Ambiental com ESP32
 * Grupo: 10
 * Disciplina: Sistemas Embarcados
 *
 * Descrição:
 * Sistema distribuído usando 1 ESP32 para monitorar condições ambientais
 * (temperatura, umidade, luminosidade) e detecção de presença em tempo real.
 *
 * Tecnologias Utilizadas:
 * - Hardware: ESP32 (NodeMCU)
 * - Firmware: FreeRTOS (Tasks e Filas)
 * - Protocolo: MQTT (PubSubClient)
 * - Broker: Mosquitto (Docker)
 * - Comunicação: Wi-Fi
 *
 * Arquitetura FreeRTOS:
 * - Task 1 (Core 0): Leitura DHT11 (temp/umidade) - 5s
 * - Task 2 (Core 0): Leitura LDR (luminosidade) - 5s
 * - Task 3 (Core 0): Leitura TCRT-5000 (entrada/saída) - 50ms
 * - Task 4 (Core 1): Publicação MQTT - 100ms
 * - Filas: Comunicação assíncrona entre tasks
 *
 * Sensores:
 * - DHT11: Temperatura e umidade
 * - LDR: Luminosidade ambiente
 * - TCRT-5000 (x2): Sensores ópticos de entrada/saída
 * - LED: Indicador visual de ocupação
 *
 * 🐳 VERSÃO COM MOSQUITTO NO DOCKER 🐳
 * Esta versão usa Mosquitto rodando em container Docker.
 * Monitoramento via terminal MQTT (sem Node-RED).
 * =====================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========== CONFIGURAÇÕES DE REDE ==========
// ALTERE ESTAS CONFIGURAÇÕES PARA SUA REDE
const char *ssid = "contingencia";              // Nome da rede Wi-Fi
const char *password = "acesso a internet 009"; // Senha da rede Wi-Fi

// ========== CONFIGURAÇÕES DO BROKER MQTT (MOSQUITTO NO DOCKER) ==========
//
// 🐳 MOSQUITTO NO DOCKER - Como configurar:
//
// 1️⃣ INSTALAR O DOCKER:
//    Windows:
//      - Baixe em: https://www.docker.com/products/docker-desktop/
//      - Instale e reinicie o PC
//      - Verifique: docker --version
//
// 2️⃣ EXECUTAR O MOSQUITTO:
//    PowerShell:
//      docker run -d --name mosquitto -p 1883:1883 eclipse-mosquitto:latest
//
//    Ou use Docker Compose (veja docker-compose.yml na pasta do projeto)
//
// 3️⃣ DESCOBRIR O IP DO COMPUTADOR COM DOCKER:
//    Windows PowerShell:
//      ipconfig
//      (Procure "Endereço IPv4" na sua rede, ex: 192.168.1.100)
//
// 4️⃣ TESTAR SE O MOSQUITTO ESTÁ FUNCIONANDO:
//    PowerShell:
//      docker exec -it mosquitto mosquitto_sub -t "teste/#" -v
//
//    Em outro terminal:
//      docker exec -it mosquitto mosquitto_pub -t "teste/msg" -m "Olá!"
//
// 5️⃣ CONFIGURAR ABAIXO:
//
// ⚠️ IMPORTANTE: ESP32 NÃO ACEITA "localhost"!
// ⚠️ Use o IP do computador onde o DOCKER COM MOSQUITTO está rodando
//
// ❌ ERRADO: const char* mqtt_server = "localhost";
// ✅ CORRETO: const char* mqtt_server = "192.168.1.100";
//
const char *mqtt_server = "172.26.192.64"; // ⬅️ IP ATUALIZADO! (use: ipconfig para ver o seu)
const uint16_t MQTT_PORT = 1883;           // Porta padrão MQTT (não precisa alterar)

// 📝 NOTA: Se você configurou senha no Mosquitto, descomente e configure:
// const char *mqtt_user = "seu_usuario";     // Usuário MQTT (se configurado)
// const char *mqtt_password = "sua_senha";   // Senha MQTT (se configurado)

// ========== CONFIGURAÇÕES DE PINOS ==========
// Configuração dos pinos GPIO do ESP32
#define DHT_PIN 4      // GPIO 4 - Sensor DHT11 (Temperatura e Umidade)
#define DHT_TYPE DHT11 // Tipo do sensor DHT
#define LDR_PIN 32     // GPIO 32 - Sensor LDR (Luminosidade) - Apenas INPUT (ADC)
#define TCRT_SENSOR1_PIN 13  // GPIO 13 - Sensor TCRT-5000 #1 (Externo - entrada da porta)
#define TCRT_SENSOR2_PIN 12  // GPIO 12 - Sensor TCRT-5000 #2 (Interno - dentro da sala)
#define LED_PIN 2      // GPIO 2 - LED indicador de ocupação

// ========== PARÂMETROS IDEAIS ==========
const float TEMP_MIN = 20.0;
const float TEMP_MAX = 25.0;
const float UMID_MIN = 40.0;
const float UMID_MAX = 60.0;

// ========== OBJETOS GLOBAIS ==========
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

// ========== VARIÁVEIS DE CONTROLE ==========
bool dhtDisponivel = false;
bool ldrDisponivel = false;
bool tcrtDisponivel = false;

// Controle de pessoas e estado da sala
int pessoasNaSala = 0;
bool salaOcupada = false;

// Variáveis para detecção de sequência dos sensores TCRT
volatile bool sensor1Ativado = false;
volatile bool sensor2Ativado = false;
volatile unsigned long tempoSensor1 = 0;
volatile unsigned long tempoSensor2 = 0;
const unsigned long TIMEOUT_SEQUENCIA = 2000; // 2 segundos para completar a sequência

// ========== FILAS FREERTOS ==========
QueueHandle_t queueTemp;
QueueHandle_t queueUmid;
QueueHandle_t queueLuz;
QueueHandle_t queueEntradaSaida; // Fila para eventos de entrada/saída (1 = entrada, -1 = saída)

// ========== FUNÇÕES DE CONEXÃO ==========

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao Mosquitto Broker (");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.print(MQTT_PORT);
    Serial.print(")...");

    // Gera um Client ID único baseado no MAC address do ESP32
    String clientId = "ESP32_Sala_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    // Tenta conectar ao Mosquitto
    // Se você configurou usuário/senha no Mosquitto, use:
    // if (client.connect(clientId.c_str(), mqtt_user, mqtt_password))
    if (client.connect(clientId.c_str()))
    {
      Serial.println(" ✓ Conectado ao Mosquitto!");
      Serial.print("Client ID: ");
      Serial.println(clientId);

      // Publica mensagem de status
      client.publish("sala/status", "ESP32 Online - Conectado ao Mosquitto");
    }
    else
    {
      Serial.print(" ✗ Falha! Código de erro: ");
      Serial.print(client.state());

      // Decodifica o erro para facilitar debug
      switch (client.state())
      {
      case -4:
        Serial.println(" (Timeout de conexão)");
        break;
      case -3:
        Serial.println(" (Conexão perdida)");
        break;
      case -2:
        Serial.println(" (Falha na conexão)");
        break;
      case -1:
        Serial.println(" (Desconectado)");
        break;
      case 1:
        Serial.println(" (Protocolo incorreto)");
        break;
      case 2:
        Serial.println(" (Client ID rejeitado)");
        break;
      case 3:
        Serial.println(" (Servidor indisponível)");
        break;
      case 4:
        Serial.println(" (Credenciais inválidas)");
        break;
      case 5:
        Serial.println(" (Não autorizado)");
        break;
      default:
        Serial.println(" (Erro desconhecido)");
        break;
      }

      Serial.println("🔄 Tentando novamente em 5 segundos...");
      Serial.println("💡 Dicas:");
      Serial.println("   - Verifique se o Mosquitto está rodando");
      Serial.println("   - Confirme o IP configurado");
      Serial.println("   - Teste: mosquitto_sub -h localhost -t \"sala/#\" -v");
      delay(5000);
    }
  }
}

// ========== DETECÇÃO AUTOMÁTICA DE SENSORES ==========
void detectarSensores()
{
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   DETECTANDO SENSORES CONECTADOS...   ║");
  Serial.println("╚════════════════════════════════════════╝");

  // Testa DHT11 (Temperatura e Umidade)
  dht.begin();
  delay(2000);
  float testTemp = dht.readTemperature();
  if (!isnan(testTemp))
  {
    dhtDisponivel = true;
    Serial.println("✓ DHT11 detectado no GPIO 15");
  }
  else
  {
    Serial.println("✗ DHT11 NÃO detectado");
  }

  // Testa LDR (Luminosidade)
  pinMode(LDR_PIN, INPUT);
  int ldrVal = analogRead(LDR_PIN);
  if (ldrVal > 0 && ldrVal < 4095)
  {
    ldrDisponivel = true;
    Serial.println("✓ LDR detectado no GPIO 34");
  }
  else
  {
    Serial.println("✗ LDR NÃO detectado");
  }

  // Testa sensores TCRT-5000
  pinMode(TCRT_SENSOR1_PIN, INPUT_PULLUP);
  pinMode(TCRT_SENSOR2_PIN, INPUT_PULLUP);
  tcrtDisponivel = true;
  Serial.println("✓ TCRT Sensor 1 (Externo) configurado no GPIO 13");
  Serial.println("✓ TCRT Sensor 2 (Interno) configurado no GPIO 12");

  // Configura LED
  pinMode(LED_PIN, OUTPUT);
  Serial.println("✓ LED configurado no GPIO 32");

  Serial.println("════════════════════════════════════════\n");
}

// ========== TASK 1: LEITURA DO DHT11 (Temperatura e Umidade) ==========
// Esta task roda no Core 0 e coleta dados do sensor DHT11 a cada 5 segundos
// Utiliza vTaskDelayUntil para garantir período constante
void taskDHT11(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(5000); // 5 segundos

  while (1)
  {
    if (dhtDisponivel)
    {
      float temp = dht.readTemperature();
      float umid = dht.readHumidity();

      if (!isnan(temp) && !isnan(umid))
      {
        // Envia dados para as filas (comunicação inter-task)
        xQueueSend(queueTemp, &temp, portMAX_DELAY);
        xQueueSend(queueUmid, &umid, portMAX_DELAY);
      }
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// ========== TASK 2: LEITURA DO LDR (Luminosidade) ==========
// Esta task roda no Core 0 e mede a luminosidade ambiente a cada 5 segundos
void taskLDR(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(5000); // 5 segundos

  while (1)
  {
    if (ldrDisponivel)
    {
      int ldrVal = analogRead(LDR_PIN);
      float luminosidade = map(ldrVal, 0, 4095, 0, 100);
      xQueueSend(queueLuz, &luminosidade, portMAX_DELAY);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// ========== TASK 3: LEITURA DOS SENSORES TCRT-5000 (Entrada/Saída) ==========
// Esta task roda no Core 0 e detecta a sequência de interrupções dos sensores
// Lógica:
//   - Sensor 1 → Sensor 2: Alguém ENTROU na sala (contador++)
//   - Sensor 2 → Sensor 1: Alguém SAIU da sala (contador--)
// Controla o LED indicador de ocupação da sala
void taskTCRT(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  bool estadoAnteriorSensor1 = HIGH; // TCRT-5000 é normalmente HIGH (sem objeto)
  bool estadoAnteriorSensor2 = HIGH;
  
  enum Estado {
    AGUARDANDO,      // Esperando primeira interrupção
    SENSOR1_ATIVO,   // Sensor 1 foi ativado primeiro
    SENSOR2_ATIVO    // Sensor 2 foi ativado primeiro
  };
  
  Estado estadoAtual = AGUARDANDO;
  unsigned long tempoInicioSequencia = 0;

  while (1)
  {
    if (tcrtDisponivel)
    {
      // Lê os sensores (LOW = objeto detectado, HIGH = sem objeto)
      bool leituraSensor1 = digitalRead(TCRT_SENSOR1_PIN);
      bool leituraSensor2 = digitalRead(TCRT_SENSOR2_PIN);

      // Detecta mudança de estado no Sensor 1 (HIGH → LOW = objeto detectado)
      if (estadoAnteriorSensor1 == HIGH && leituraSensor1 == LOW)
      {
        if (estadoAtual == AGUARDANDO)
        {
          // Primeira detecção: Sensor 1 (Externo)
          estadoAtual = SENSOR1_ATIVO;
          tempoInicioSequencia = millis();
          Serial.println("🔵 Sensor 1 (EXTERNO) ativado - aguardando Sensor 2...");
        }
        else if (estadoAtual == SENSOR2_ATIVO)
        {
          // Sequência: Sensor 2 → Sensor 1 = SAÍDA
          if ((millis() - tempoInicioSequencia) <= TIMEOUT_SEQUENCIA)
          {
            int evento = -1; // -1 indica saída
            xQueueSend(queueEntradaSaida, &evento, portMAX_DELAY);
            Serial.println("🚪⬅️  SAÍDA DETECTADA (Sensor 2 → Sensor 1)");
          }
          estadoAtual = AGUARDANDO;
        }
      }

      // Detecta mudança de estado no Sensor 2 (HIGH → LOW = objeto detectado)
      if (estadoAnteriorSensor2 == HIGH && leituraSensor2 == LOW)
      {
        if (estadoAtual == AGUARDANDO)
        {
          // Primeira detecção: Sensor 2 (Interno)
          estadoAtual = SENSOR2_ATIVO;
          tempoInicioSequencia = millis();
          Serial.println("🟢 Sensor 2 (INTERNO) ativado - aguardando Sensor 1...");
        }
        else if (estadoAtual == SENSOR1_ATIVO)
        {
          // Sequência: Sensor 1 → Sensor 2 = ENTRADA
          if ((millis() - tempoInicioSequencia) <= TIMEOUT_SEQUENCIA)
          {
            int evento = 1; // 1 indica entrada
            xQueueSend(queueEntradaSaida, &evento, portMAX_DELAY);
            Serial.println("🚪➡️  ENTRADA DETECTADA (Sensor 1 → Sensor 2)");
          }
          estadoAtual = AGUARDANDO;
        }
      }

      // Reset por timeout (se passar muito tempo, cancela a sequência)
      if (estadoAtual != AGUARDANDO && (millis() - tempoInicioSequencia) > TIMEOUT_SEQUENCIA)
      {
        Serial.println("⏱️  Timeout - sequência cancelada");
        estadoAtual = AGUARDANDO;
      }

      // Atualiza estados anteriores
      estadoAnteriorSensor1 = leituraSensor1;
      estadoAnteriorSensor2 = leituraSensor2;

      // Atualiza LED baseado na ocupação atual
      digitalWrite(LED_PIN, salaOcupada ? HIGH : LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // Verifica a cada 50ms (mais rápido que PIR)
  }
}

// ========== TASK 4: PUBLICAÇÃO MQTT ==========
// Esta task roda no Core 1 (separado) e publica dados via MQTT
// Consome dados das filas e envia para o broker Mosquitto (Docker)
// Monitoramento via terminal MQTT (sem dashboard web)
// Implementa lógica de alertas e sugestões inteligentes
void taskMQTT(void *pvParameters)
{
  float temperatura, umidade, luminosidade;
  int eventoEntradaSaida; // 1 = entrada, -1 = saída

  while (1)
  {
    // Mantém conexão MQTT com Mosquitto
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();

    // Processa temperatura
    if (xQueueReceive(queueTemp, &temperatura, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(temperatura, 6, 2, buffer);

      // Publica no Mosquitto Docker (QoS 0 = Fire and Forget)
      // Para monitorar: docker exec -it mosquitto mosquitto_sub -t "sala/#" -v
      if (client.publish("sala/temperatura", buffer))
      {
        Serial.printf("📊 [MQTT] Temperatura: %.2f °C → Mosquitto\n", temperatura);
      }

      // Alertas de temperatura
      if (temperatura < TEMP_MIN || temperatura > TEMP_MAX)
      {
        char alerta[100];
        snprintf(alerta, 100, "ALERTA: Temperatura fora do ideal (%.2f °C)", temperatura);
        client.publish("sala/alertas", alerta);
        Serial.println("⚠️  " + String(alerta));
      }

      // Sugestão de ar-condicionado
      if (temperatura > TEMP_MAX)
      {
        client.publish("sala/sugestao_ac", "LIGAR");
        Serial.println("❄️  Sugestão: LIGAR ar-condicionado");
      }
      else if (temperatura < TEMP_MIN + 1)
      {
        client.publish("sala/sugestao_ac", "DESLIGAR");
        Serial.println("❄️  Sugestão: DESLIGAR ar-condicionado");
      }
    }

    // Processa umidade
    if (xQueueReceive(queueUmid, &umidade, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(umidade, 6, 2, buffer);

      if (client.publish("sala/umidade", buffer))
      {
        Serial.printf("💧 [MQTT] Umidade: %.2f %% → Mosquitto\n", umidade);
      }

      // Alertas de umidade
      if (umidade < UMID_MIN || umidade > UMID_MAX)
      {
        char alerta[100];
        snprintf(alerta, 100, "ALERTA: Umidade fora do ideal (%.2f %%)", umidade);
        client.publish("sala/alertas", alerta);
        Serial.println("⚠️  " + String(alerta));
      }
    }

    // Processa luminosidade
    if (xQueueReceive(queueLuz, &luminosidade, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(luminosidade, 6, 2, buffer);

      if (client.publish("sala/luminosidade", buffer))
      {
        Serial.printf("💡 [MQTT] Luminosidade: %.2f %% → Mosquitto\n", luminosidade);
      }
    }

    // Processa eventos de entrada/saída
    if (xQueueReceive(queueEntradaSaida, &eventoEntradaSaida, 0) == pdTRUE)
    {
      if (eventoEntradaSaida == 1)
      {
        // ENTRADA detectada
        pessoasNaSala++;
        salaOcupada = (pessoasNaSala > 0);
        
        char buffer[50];
        snprintf(buffer, 50, "%d", pessoasNaSala);
        
        client.publish("sala/entrada", "DETECTADA");
        client.publish("sala/pessoas", buffer);
        client.publish("sala/ocupacao", salaOcupada ? "OCUPADA" : "VAZIA");
        
        Serial.printf("👤➡️  [MQTT] ENTRADA - Pessoas na sala: %d → Mosquitto\n", pessoasNaSala);
      }
      else if (eventoEntradaSaida == -1)
      {
        // SAÍDA detectada
        pessoasNaSala--;
        if (pessoasNaSala < 0) pessoasNaSala = 0; // Proteção contra valores negativos
        
        salaOcupada = (pessoasNaSala > 0);
        
        char buffer[50];
        snprintf(buffer, 50, "%d", pessoasNaSala);
        
        client.publish("sala/saida", "DETECTADA");
        client.publish("sala/pessoas", buffer);
        client.publish("sala/ocupacao", salaOcupada ? "OCUPADA" : "VAZIA");
        
        Serial.printf("�⬅️  [MQTT] SAÍDA - Pessoas na sala: %d → Mosquitto\n", pessoasNaSala);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Loop rápido
  }
} // ========== SETUP ==========
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════╗");
  Serial.println("║  Sistema de Monitoramento de Sala Inteligente ║");
  Serial.println("║              GRUPO 10 - ESP32                  ║");
  Serial.println("╚════════════════════════════════════════════════╝");

  // Detecta sensores disponíveis
  detectarSensores();

  // Conecta Wi-Fi
  setup_wifi();

  // Configura MQTT
  client.setServer(mqtt_server, MQTT_PORT);
  reconnect();

  // Cria as filas
  queueTemp = xQueueCreate(5, sizeof(float));
  queueUmid = xQueueCreate(5, sizeof(float));
  queueLuz = xQueueCreate(5, sizeof(float));
  queueEntradaSaida = xQueueCreate(10, sizeof(int)); // Maior capacidade para eventos de entrada/saída

  Serial.println("\n╔═══════════════════════════════════════╗");
  Serial.println("║   CRIANDO TASKS FREERTOS...          ║");
  Serial.println("╚═══════════════════════════════════════╝");

  // Cria tasks apenas para sensores disponíveis (economia de recursos)
  if (dhtDisponivel)
  {
    xTaskCreatePinnedToCore(
        taskDHT11, // Função da task
        "DHT11",   // Nome da task (para debug)
        4096,      // Stack size (bytes)
        NULL,      // Parâmetros
        1,         // Prioridade (1 = normal)
        NULL,      // Handle (não usado)
        0          // Core 0 (sensor tasks)
    );
    Serial.println("✓ Task DHT11 criada no Core 0");
  }

  if (ldrDisponivel)
  {
    xTaskCreatePinnedToCore(
        taskLDR,
        "LDR",
        2048,
        NULL,
        1,
        NULL,
        0);
    Serial.println("✓ Task LDR criada no Core 0");
  }

  if (tcrtDisponivel)
  {
    xTaskCreatePinnedToCore(
        taskTCRT, // Função da task dos sensores TCRT
        "TCRT",   // Nome da task
        4096,     // Stack size (maior por causa da lógica de sequência)
        NULL,     // Parâmetros
        2,        // Prioridade ALTA (detecção crítica)
        NULL,     // Handle
        0         // Core 0
    );
    Serial.println("✓ Task TCRT criada no Core 0 (prioridade ALTA)");
  }

  // Task MQTT sempre criada - roda no Core 1 (isolado)
  xTaskCreatePinnedToCore(
      taskMQTT, // Função da task
      "MQTT",   // Nome da task
      4096,     // Stack size (maior por causa de buffers)
      NULL,     // Parâmetros
      2,        // Prioridade ALTA (comunicação prioritária)
      NULL,     // Handle
      1         // Core 1 (comunicação isolada)
  );
  Serial.println("✓ Task MQTT criada no Core 1 (prioridade ALTA)");

  Serial.println("═══════════════════════════════════════");
  Serial.println("\n🚀 Sistema iniciado com sucesso!");
  Serial.println("📡 Monitorando sensores e publicando via MQTT...");
  Serial.println("🐳 Broker: Mosquitto Docker (" + String(mqtt_server) + ":" + String(MQTT_PORT) + ")");
  Serial.println("\n💡 Para monitorar os dados:");
  Serial.println("   docker exec -it mosquitto mosquitto_sub -t \"sala/#\" -v\n");
}

// ========== LOOP PRINCIPAL ==========
// O loop() fica vazio pois o FreeRTOS gerencia todas as tasks automaticamente
// Cada task roda em seu próprio contexto de forma paralela
void loop()
{
  // Suspende esta task indefinidamente
  // Todo o trabalho é feito pelas tasks do FreeRTOS
  vTaskDelay(portMAX_DELAY);
}