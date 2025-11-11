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
 * - Comunicação: Wi-Fi
 * - Dashboard: Node-RED
 *
 * Arquitetura FreeRTOS:
 * - Task 1 (Core 0): Leitura DHT11 (temp/umidade) - 5s
 * - Task 2 (Core 0): Leitura LDR (luminosidade) - 5s
 * - Task 3 (Core 0): Leitura PIR (presença) - 500ms
 * - Task 4 (Core 1): Publicação MQTT - 100ms
 * - Filas: Comunicação assíncrona entre tasks
 *
 * Sensores:
 * - DHT11: Temperatura e umidade
 * - LDR: Luminosidade ambiente
 * - PIR: Detecção de movimento/presença
 * - LED: Indicador visual de ocupação
 * =====================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========== CONFIGURAÇÕES DE REDE ==========
// ALTERE ESTAS CONFIGURAÇÕES PARA SUA REDE
const char *ssid = "uaifai-tiradentes";    // Nome da rede Wi-Fi
const char *password = "bemvindoaocesar";  // Senha da rede Wi-Fi
// const char *mqtt_server = "172.26.70.247"; // IP do Broker MQTT (Raspberry Pi ou PC local)
IPAddress MQTT_IP(172, 26, 70, 247);
// const int mqtt_port = 1883;
const uint16_t MQTT_PORT = 1883;              // Porta padrão MQTT

// ========== CONFIGURAÇÕES DE PINOS ==========
// Configuração dos pinos GPIO do ESP32
#define DHT_PIN 4     // GPIO 4 - Sensor DHT11 (Temperatura e Umidade)
#define DHT_TYPE DHT11 // Tipo do sensor DHT
#define LDR_PIN 32     // GPIO 32 - Sensor LDR (Luminosidade) - Apenas INPUT (ADC)
#define PIR_PIN 13     // GPIO 13 - Sensor PIR (Presença/Movimento)
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
bool pirDisponivel = false;

// ========== FILAS FREERTOS ==========
QueueHandle_t queueTemp;
QueueHandle_t queueUmid;
QueueHandle_t queueLuz;
QueueHandle_t queuePresenca;

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
    Serial.print("Conectando ao MQTT...");

    String clientId = "ESP32_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str()))
    {
      Serial.println("Conectado!");
    }
    else
    {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" - Tentando novamente em 5s");
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

  // Testa PIR (Presença)
  pinMode(PIR_PIN, INPUT);
  pirDisponivel = true; // PIR sempre disponível se configurado
  Serial.println("✓ PIR configurado no GPIO 13");

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

// ========== TASK 3: LEITURA DO PIR (Presença) + Controle de LED ==========
// Esta task roda no Core 0 e detecta presença a cada 500ms
// Controla o LED indicador de ocupação da sala
void taskPIR(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  bool estadoAnterior = false;

  while (1)
  {
    if (pirDisponivel)
    {
      bool presenca = digitalRead(PIR_PIN);

      // Atualiza LED (acende se ocupado, apaga se vazio)
      digitalWrite(LED_PIN, presenca ? HIGH : LOW);

      // Envia para fila apenas se o estado mudou (evita publicações desnecessárias)
      if (presenca != estadoAnterior)
      {
        xQueueSend(queuePresenca, &presenca, portMAX_DELAY);
        estadoAnterior = presenca;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(500)); // Verifica a cada 500ms
  }
}

// ========== TASK 4: PUBLICAÇÃO MQTT ==========
// Esta task roda no Core 1 (separado) e publica dados via MQTT
// Consome dados das filas e envia para o broker MQTT (Node-RED)
// Implementa lógica de alertas e sugestões inteligentes
void taskMQTT(void *pvParameters)
{
  float temperatura, umidade, luminosidade;
  bool presenca;

  while (1)
  {
    // Mantém conexão MQTT
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
      client.publish("sala/temperatura", buffer);
      Serial.printf("📊 Temperatura: %.2f °C\n", temperatura);

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
      client.publish("sala/umidade", buffer);
      Serial.printf("💧 Umidade: %.2f %%\n", umidade);

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
      client.publish("sala/luminosidade", buffer);
      Serial.printf("💡 Luminosidade: %.2f %%\n", luminosidade);
    }

    // Processa presença
    if (xQueueReceive(queuePresenca, &presenca, 0) == pdTRUE)
    {
      if (presenca)
      {
        client.publish("sala/presenca", "DETECTADA");
        client.publish("sala/ocupacao", "OCUPADA");
        Serial.println("👤 PRESENÇA DETECTADA - Sala OCUPADA - LED LIGADO");
      }
      else
      {
        client.publish("sala/presenca", "NAO_DETECTADA");
        client.publish("sala/ocupacao", "VAZIA");
        Serial.println("🚪 Presença NÃO detectada - Sala VAZIA - LED DESLIGADO");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Loop rápido
  }
}

// ========== SETUP ==========
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
  client.setServer(MQTT_IP, MQTT_PORT);
  reconnect();

  // Cria as filas
  queueTemp = xQueueCreate(5, sizeof(float));
  queueUmid = xQueueCreate(5, sizeof(float));
  queueLuz = xQueueCreate(5, sizeof(float));
  queuePresenca = xQueueCreate(5, sizeof(bool));

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

  if (pirDisponivel)
  {
    xTaskCreatePinnedToCore(
        taskPIR, // Função da task
        "PIR",   // Nome da task
        2048,    // Stack size
        NULL,    // Parâmetros
        1,       // Prioridade
        NULL,    // Handle
        0        // Core 0
    );
    Serial.println("✓ Task PIR criada no Core 0");
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
  Serial.println("🌐 Dashboard disponível no Node-RED\n");
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