/*
 * =====================================================================
 * Sistema de Monitoramento de Sala Inteligente - Grupo 10
 * ESP32 + FreeRTOS + MQTT + Docker Mosquitto
 * =====================================================================
 * Sensores: DHT11, LDR, TCRT-5000 (x2), LED
 * Tasks: DHT11, LDR, TCRT, LED, MQTT
 * =====================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========== CONFIGURAÇÕES ==========
const char *ssid = "uaifai-tiradentes";
const char *password = "bemvindoaocesar";
const char *mqtt_server = "172.26.70.247";
const uint16_t MQTT_PORT = 1883;

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LDR_PIN 32
#define TCRT_SENSOR1_PIN 13 // Externo
#define TCRT_SENSOR2_PIN 14 // Interno
#define LED_PIN 2

const float TEMP_MIN = 20.0, TEMP_MAX = 25.0;
const float UMID_MIN = 40.0, UMID_MAX = 60.0;
const unsigned long TIMEOUT_SEQUENCIA = 2000;
const unsigned long DEBOUNCE_DELAY = 50;

// ========== OBJETOS GLOBAIS ==========
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

bool dhtDisponivel = false, ldrDisponivel = false, tcrtDisponivel = false;
int pessoasNaSala = 0;
bool salaOcupada = false;

QueueHandle_t queueTemp, queueUmid, queueLuz, queueEntradaSaida;

// ========== CONEXÃO WI-FI ==========
void setup_wifi()
{
  Serial.print("\nConectando ao Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✓ WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ========== CONEXÃO MQTT ==========
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando MQTT...");
    String clientId = "ESP32_Sala_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str()))
    {
      Serial.println(" ✓ Conectado!");
      client.publish("sala/status", "ESP32 Online");
    }
    else
    {
      Serial.print(" ✗ Falha (");
      Serial.print(client.state());
      Serial.println(") - Tentando em 5s...");
      delay(5000);
    }
  }
}

// ========== DETECÇÃO DE SENSORES ==========
void detectarSensores()
{
  Serial.println("\n╔═══ DETECTANDO SENSORES ═══╗");

  dht.begin();
  delay(2000);
  dhtDisponivel = !isnan(dht.readTemperature());
  Serial.println(dhtDisponivel ? "✓ DHT11" : "✗ DHT11");

  pinMode(LDR_PIN, INPUT);
  int ldrVal = analogRead(LDR_PIN);
  ldrDisponivel = (ldrVal > 0 && ldrVal < 4095);
  Serial.println(ldrDisponivel ? "✓ LDR" : "✗ LDR");

  pinMode(TCRT_SENSOR1_PIN, INPUT_PULLUP);
  pinMode(TCRT_SENSOR2_PIN, INPUT_PULLUP);
  tcrtDisponivel = true;
  Serial.println("✓ TCRT Sensor 1 (Externo)");
  Serial.println("✓ TCRT Sensor 2 (Interno)");

  pinMode(LED_PIN, OUTPUT);
  Serial.println("✓ LED");
  Serial.println("╚═══════════════════════════╝\n");
}

// ========== TASKS ==========
void taskDHT11(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    if (dhtDisponivel)
    {
      float temp = dht.readTemperature();
      float umid = dht.readHumidity();
      if (!isnan(temp) && !isnan(umid))
      {
        xQueueSend(queueTemp, &temp, portMAX_DELAY);
        xQueueSend(queueUmid, &umid, portMAX_DELAY);
      }
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));
  }
}

void taskLDR(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    if (ldrDisponivel)
    {
      int ldrVal = analogRead(LDR_PIN);
      ldrVal = (ldrVal - 4095) * -1;
      float luminosidade = map(ldrVal, 0, 4095, 0, 100);
      xQueueSend(queueLuz, &luminosidade, portMAX_DELAY);
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));
  }
}

// Task TCRT: Detecta sequência Sensor1→Sensor2 (ENTRADA) ou Sensor2→Sensor1 (SAÍDA)
void taskTCRT(void *pvParameters)
{
  bool estadoAnt1 = HIGH, estadoAnt2 = HIGH;
  enum
  {
    AGUARDANDO,
    SENSOR1_ATIVO,
    SENSOR2_ATIVO
  } estado = AGUARDANDO;
  unsigned long tempoInicio = 0, ultLeitura1 = 0, ultLeitura2 = 0;

  while (1)
  {
    if (tcrtDisponivel)
    {
      unsigned long agora = millis();
      bool leitura1 = digitalRead(TCRT_SENSOR1_PIN);
      bool leitura2 = digitalRead(TCRT_SENSOR2_PIN);

      // Sensor 1 ativado (borda de descida)
      if (estadoAnt1 == HIGH && leitura1 == LOW && (agora - ultLeitura1) > DEBOUNCE_DELAY)
      {
        ultLeitura1 = agora;
        if (estado == AGUARDANDO)
        {
          estado = SENSOR1_ATIVO;
          tempoInicio = agora;
          Serial.println("\n🔵 Sensor 1 (EXTERNO) ativado");
        }
        else if (estado == SENSOR2_ATIVO && (agora - tempoInicio) <= TIMEOUT_SEQUENCIA)
        {
          int evento = -1; // SAÍDA
          xQueueSend(queueEntradaSaida, &evento, 0);
          Serial.println("🚪⬅️  SAÍDA (S2→S1)");
          estado = AGUARDANDO;
        }
      }

      // Sensor 2 ativado (borda de descida)
      if (estadoAnt2 == HIGH && leitura2 == LOW && (agora - ultLeitura2) > DEBOUNCE_DELAY)
      {
        ultLeitura2 = agora;
        if (estado == AGUARDANDO)
        {
          estado = SENSOR2_ATIVO;
          tempoInicio = agora;
          Serial.println("\n🟢 Sensor 2 (INTERNO) ativado");
        }
        else if (estado == SENSOR1_ATIVO && (agora - tempoInicio) <= TIMEOUT_SEQUENCIA)
        {
          int evento = 1; // ENTRADA
          xQueueSend(queueEntradaSaida, &evento, 0);
          Serial.println("🚪➡️  ENTRADA (S1→S2)");
          estado = AGUARDANDO;
        }
      }

      // Timeout - reseta estado
      if (estado != AGUARDANDO && (agora - tempoInicio) > TIMEOUT_SEQUENCIA)
      {
        Serial.println("⏱️  Timeout");
        estado = AGUARDANDO;
      }

      estadoAnt1 = leitura1;
      estadoAnt2 = leitura2;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskLED(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);
  while (1)
  {
    digitalWrite(LED_PIN, salaOcupada ? HIGH : LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void taskMQTT(void *pvParameters)
{
  float temperatura, umidade, luminosidade;
  int eventoEntradaSaida;
  unsigned long ultimaTentativaReconexao = 0;
  const unsigned long INTERVALO_RECONEXAO = 5000; // 5 segundos

  while (1)
  {
    // Reconexão MQTT com controle de tempo
    if (!client.connected())
    {
      unsigned long agora = millis();
      if (agora - ultimaTentativaReconexao >= INTERVALO_RECONEXAO)
      {
        ultimaTentativaReconexao = agora;
        Serial.print("\n🔄 Reconectando MQTT...");
        String clientId = "ESP32_Sala_" + String((uint32_t)ESP.getEfuseMac(), HEX);
        
        if (client.connect(clientId.c_str()))
        {
          Serial.println(" ✓ Reconectado!");
          client.publish("sala/status", "ESP32 Reconectado");
        }
        else
        {
          Serial.printf(" ✗ Falha (%d)\n", client.state());
        }
      }
      vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1s antes de tentar processar filas
      continue;
    }
    
    client.loop();

    // Temperatura
    if (xQueueReceive(queueTemp, &temperatura, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(temperatura, 6, 2, buffer);
      client.publish("sala/temperatura", buffer);
      Serial.printf("📊 Temp: %.2f°C\n", temperatura);

      if (temperatura < TEMP_MIN || temperatura > TEMP_MAX)
      {
        client.publish("sala/alertas", "Temperatura fora do ideal");
        Serial.println("⚠️  Alerta: Temperatura!");
      }
    }

    // Umidade
    if (xQueueReceive(queueUmid, &umidade, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(umidade, 6, 2, buffer);
      client.publish("sala/umidade", buffer);
      Serial.printf("💧 Umid: %.2f%%\n", umidade);

      if (umidade < UMID_MIN || umidade > UMID_MAX)
      {
        client.publish("sala/alertas", "Umidade fora do ideal");
        Serial.println("⚠️  Alerta: Umidade!");
      }
    }

    // Luminosidade
    if (xQueueReceive(queueLuz, &luminosidade, 0) == pdTRUE)
    {
      char buffer[10];
      dtostrf(luminosidade, 6, 2, buffer);
      client.publish("sala/luminosidade", buffer);
      Serial.printf("💡 Luz: %.2f%%\n", luminosidade);
    }

    // Entrada/Saída
    if (xQueueReceive(queueEntradaSaida, &eventoEntradaSaida, 0) == pdTRUE)
    {
      if (eventoEntradaSaida == 1)
      {
        pessoasNaSala++;
        salaOcupada = (pessoasNaSala > 0);
        char buffer[50];
        snprintf(buffer, 50, "%d", pessoasNaSala);
        client.publish("sala/entrada", "DETECTADA");
        client.publish("sala/pessoas", buffer);
        client.publish("sala/ocupacao", salaOcupada ? "OCUPADA" : "VAZIA");
        Serial.printf("👤➡️  ENTRADA - %d pessoas\n", pessoasNaSala);
      }
      else if (eventoEntradaSaida == -1)
      {
        pessoasNaSala--;
        if (pessoasNaSala < 0)
          pessoasNaSala = 0;
        salaOcupada = (pessoasNaSala > 0);
        char buffer[50];
        snprintf(buffer, 50, "%d", pessoasNaSala);
        client.publish("sala/saida", "DETECTADA");
        client.publish("sala/pessoas", buffer);
        client.publish("sala/ocupacao", salaOcupada ? "OCUPADA" : "VAZIA");
        Serial.printf("👤⬅️  SAÍDA - %d pessoas\n", pessoasNaSala);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
// ========== SETUP ==========
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n╔═══════════════════════════════════╗");
  Serial.println("║  Sala Inteligente - Grupo 10     ║");
  Serial.println("╚═══════════════════════════════════╝");

  detectarSensores();
  setup_wifi();
  
  // Configura MQTT com timeouts maiores
  client.setServer(mqtt_server, MQTT_PORT);
  client.setKeepAlive(60);        // Keep-alive de 60s
  client.setSocketTimeout(15);    // Timeout de 15s
  reconnect();

  // Cria filas
  queueTemp = xQueueCreate(5, sizeof(float));
  queueUmid = xQueueCreate(5, sizeof(float));
  queueLuz = xQueueCreate(5, sizeof(float));
  queueEntradaSaida = xQueueCreate(10, sizeof(int));

  Serial.println("\n╔═══ CRIANDO TASKS ═══╗");

  // Tasks Core 0
  if (dhtDisponivel)
  {
    xTaskCreatePinnedToCore(taskDHT11, "DHT11", 4096, NULL, 1, NULL, 0);
    Serial.println("✓ DHT11 (Core 0)");
  }

  if (ldrDisponivel)
  {
    xTaskCreatePinnedToCore(taskLDR, "LDR", 2048, NULL, 1, NULL, 0);
    Serial.println("✓ LDR (Core 0)");
  }

  if (tcrtDisponivel)
  {
    xTaskCreatePinnedToCore(taskTCRT, "TCRT", 4096, NULL, 2, NULL, 0);
    Serial.println("✓ TCRT (Core 0)");
    xTaskCreatePinnedToCore(taskLED, "LED", 2048, NULL, 1, NULL, 0);
    Serial.println("✓ LED (Core 0)");
  }

  // Task Core 1
  xTaskCreatePinnedToCore(taskMQTT, "MQTT", 4096, NULL, 2, NULL, 1);
  Serial.println("✓ MQTT (Core 1)");

  Serial.println("╚═════════════════════╝");
  Serial.println("\n🚀 Sistema iniciado!");
  Serial.printf("🐳 Broker: %s:%d\n\n", mqtt_server, MQTT_PORT);
}

void loop()
{
  vTaskDelay(portMAX_DELAY);
}