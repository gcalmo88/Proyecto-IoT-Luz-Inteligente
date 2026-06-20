# Sistema de Monitoreo y Control IoT mediante MQTT: Luz Inteligente

## Introducción y Contexto
En el paradigma del Internet de las Cosas (IoT), la comunicación eficiente, ligera y desacoplada entre dispositivos es fundamental. Este proyecto implementa una arquitectura IoT bidireccional basada en el protocolo MQTT, integrando adquisición de telemetría en el Edge (ESP32), un agente de mensajería (Mosquitto) y un dashboard de analítica y control (Node-RED).

**Institución:** Escuela Superior de Cómputo (ESCOM) - IPN  

**Integrantes del Equipo:** 
-- Calderon Mora Gustavo Adolfo
-- Mendieta Gonzales Diego

## Arquitectura del Sistema
El sistema cumple estrictamente con las capas requeridas:
1. **Capa Edge (Publisher/Subscriber):** Microcontrolador ESP32 programado en C++ (Arduino Framework) que adquiere datos de un sensor LDR y controla actuadores locales (LEDs) según las órdenes recibidas.
2. **Capa de Red (Broker):** Servidor local basado en la imagen `eclipse-mosquitto:2` desplegado mediante contenedores de Docker.
3. **Capa de Aplicación (Dashboard):** Interfaz gráfica implementada en Node-RED para la visualización de datos históricos y actuación remota.

## Tópicos MQTT y Calidad de Servicio (QoS)
El sistema opera bajo **QoS 0** utilizando los siguientes tópicos jerárquicos:
* **Telemetría (Sensor):** `escom/iot/equipo1/sensor/luz` (Payload: Entero `0-1023`)
* **Control (Actuador):** `escom/iot/equipo1/actuador/led` (Payload: String `Encendido` / `Apagado`)

## Diagrama de Conexiones (Hardware)
El circuito se compone de un divisor de voltaje para el LDR y dos LEDs indicadores (Tx y Actuador Inteligente) alimentados a 3.3V desde el ESP32.

## Requisitos Previos

**Hardware**
* Microcontrolador ESP32 (cualquier variante DevKit).
* 1x Sensor de luz (LDR) y 1x Resistencia de 10kΩ.
* 2x LEDs (Tx y Actuador) y 2x Resistencias limitadoras (220Ω - 330Ω).
* Smartphone con capacidad de anclaje de red (Mobile Hotspot) para entornos con restricciones de red.

**Software e Infraestructura**
* [Arduino IDE](https://www.arduino.cc/en/software) configurado para la placa ESP32 y con la librería `PubSubClient` instalada.
* [Docker Desktop](https://www.docker.com/products/docker-desktop/) ejecutándose en la máquina host.
* Entorno de [Node-RED](https://nodered.org/) con el paquete `node-red-dashboard` instalado.

---

## Instrucciones de Despliegue

### 1. Preparación de la Red (Entornos Restringidos / Universidad)
Dado que las redes institucionales suelen bloquear puertos MQTT (1883) o aislar clientes, se recomienda ejecutar el sistema en una Red de Área Local (LAN) privada:
1. Activar la Zona WiFi (Mobile Hotspot) desde un smartphone.
2. Conectar la máquina host (laptop) a esta red temporal.
3. Obtener la nueva dirección IPv4 asignada a la máquina (mediante el comando `ipconfig` en Windows).

### 2. Despliegue del Broker Mosquitto
Navega a la carpeta `/infraestructura` y levanta el contenedor en segundo plano:
```bash
docker-compose up -d
```
Nota: El archivo mosquitto/config/mosquitto.conf ya está preconfigurado con listener 1883 0.0.0.0 y allow_anonymous true para aceptar las conexiones del ESP32 dentro de la LAN.

### 3. Configuración del Dashboard (Node-RED)

    Inicia el servidor de Node-RED desde la terminal ejecutando node-red.

    Accede al editor visual en http://localhost:1880.

    Importa el archivo flows.json ubicado en la carpeta /node-red.

    Haz doble clic en el nodo de configuración del servidor MQTT (Mosquitto Local) y actualiza el campo Server con la dirección IPv4 obtenida en el Paso 1.

    Presiona Deploy. El dashboard estará disponible en http://localhost:1880/ui.

### 4. Flasheo del Nodo Embebido (ESP32)

    Abre /esp32_nodo/Sensor.ino en Arduino IDE.

    Actualiza las variables de red:

        ssid: Nombre de la Zona WiFi.

        password: Contraseña de la Zona WiFi.

        mqtt_server: La misma dirección IPv4 configurada en Node-RED.

    Conecta el ESP32, selecciona el puerto COM correcto y presiona Subir. (Si la consola se detiene en Connecting..., mantén presionado el botón BOOT de la placa física).