#include <WiFi.h>
#include <PubSubClient.h>

// --- Configuración de Red ---
const char* ssid = "INFINITUMF7B2";       // nombre de wifi
const char* password = "eM9pgg3cAv";

// --- Configuración MQTT ---
const char* mqtt_server = "192.168.1.66";
const int mqtt_port = 1883;
const char* topic_pub_luz = "escom/iot/equipo1/sensor/luz"; // Tópico para publicar 
const char* topic_sub_led = "escom/iot/equipo1/actuador/led"; // Tópico para suscribirse 

// --- Definición de Pines ---
const int ldrPin = 34;
const int ledTxPin = 2;       // LED parpadeo de transmisión (el original)
const int ledActuadorPin = 4; // NUEVO: LED de la Luz Inteligente

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("\nConectando a la red WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

// --- NUEVO: Función Callback para recibir mensajes MQTT ---
void callback(char* topic, byte* payload, unsigned int length) {
  // 1. Convertir el payload a un String legible
  String mensaje;
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }
  
  Serial.print("\n>>> Mensaje recibido en [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensaje);

  // 2. Lógica del Actuador
  if (String(topic) == topic_sub_led) {
    // Los textos coinciden con lo que Node-RED envía desde la "function 3"
    if (mensaje == "Encendido") {
      digitalWrite(ledActuadorPin, HIGH);
      Serial.println("    Acción: Encender Luz Inteligente");
    } 
    else if (mensaje == "Apagado") {
      digitalWrite(ledActuadorPin, LOW);
      Serial.println("    Acción: Apagar Luz Inteligente");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32_Equipo1_";
    clientId += String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("¡Conectado al broker!");
      // IMPORTANTE: Suscribirse al tópico justo después de conectar
      client.subscribe(topic_sub_led);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  pinMode(ledTxPin, OUTPUT);
  pinMode(ledActuadorPin, OUTPUT); // Inicializamos el nuevo LED
  
  // Apagar el LED de la luz inteligente por defecto
  digitalWrite(ledActuadorPin, LOW);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Le decimos a la librería qué función usar al recibir datos
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Crucial: Esto permite que el ESP32 escuche los mensajes entrantes

  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    int ldrValue = analogRead(ldrPin);
    int ldrMapeado = map(ldrValue, 0, 4095, 0, 1023);
    String payload = String(ldrMapeado);
    
    Serial.print("Publicando nivel de luz: ");
    Serial.println(payload);
    
    client.publish(topic_pub_luz, payload.c_str());
    
    // Testigo visual de transmisión
    digitalWrite(ledTxPin, HIGH);
    delay(50);
    digitalWrite(ledTxPin, LOW);
  }
}