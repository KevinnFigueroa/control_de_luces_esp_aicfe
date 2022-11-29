//#include <WiFi.h>
#include <WiFi.h>

#include <WebSocketsServer.h>
#include <string.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define MQTTpubQos 2


/* --- RELES --- */

//
#define LUZ_1 4
#define LUZ_2 13
#define LUZ_3 14
#define LUZ_4 16
#define LUZ_5 17
#define LUZ_6 18
#define LUZ_7 19
#define LUZ_8 21
#define LUZ_9 22
#define LUZ_10 23
#define LUZ_11 25
#define LUZ_12 26
#define LUZ_13 27
#define LUZ_14 32
#define LUZ_15 33

/* --- FIN RELES --- */

WiFiClient espClient;
PubSubClient client(espClient);
WebSocketsServer webSocket = WebSocketsServer(81);

// Esto debe guardarse en la memoria EEPROM para restaurar internet en caso de corte de luz
const char* ssid = "Familia Figueroa";
const char* password = "18520182kevin";

// MQTT Broker
const char *mqtt_broker = "192.168.0.16";
const char *topic = "aicfe/light_control/esp_1";
const char *mqtt_username = "esp32/client";
const char *mqtt_password = "public";
const char *will_topic = "";
const int mqtt_port = 1883;

// Esto debe tener un id Unico para identificar como red unica
const char* APssid = "Dómotica iglesia AICFE";
const char* APpassword = "123456789";

/*
  TODO:

  ACA ME DEBE LLEGAR EN REALIDAD EL NUMERO DE PIN QUE DEBO MODIFICAR, ENTONCES NO TENGO QUE TENER IDENTIFICADORES,
  SI NO QUE MODIFICO DIRECTAMENTE EL INT DEL PIN QUE ME LLEGA DESDE MQTT

  DE ESTA MANERA TAMBIEN SE VA A FACILITAR A LA HORA DE ENVIAR DESDE EL ESP A MQTT EL VALOR DEL PIN DEL OUTPUT DE LA TECLA QUE SE MODIFICÓ ( COMUNICACIÓN BIDIRECCIONAL)

*/


void changeState(int identifier, int state) {

  digitalWrite(identifier, state);
  Serial.print("Cambiamos el estado del pin ");
  Serial.print(identifier);
  Serial.print(" a ");
  Serial.print(state);

  return;
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  String stringJson = String(( char *) payload);

  // Leer Json
  StaticJsonDocument<192> doc;

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Responder Json
  StaticJsonDocument<192> response;
  char responseBuffer[256];

  String eventName = doc["event_name"];

  if (eventName == "/change_state_response") {
    Serial.print("Entró al change state response");

    // Este evento no nos interesa
    return;
  }

  if (eventName == "/status_response") {
    Serial.print("Entró al status response");

    // Este evento no nos interesa
    return;
  }

  if (eventName == "/get_status") {
    // Los chars que envio por json enrealidad los tengo que recibir como strings
    
    Serial.print("Entró al evento get Status");

    const int identifier = doc["message"]["pin"];

    response["event_name"] = "/status_response";
    response["message"]["pin"] = identifier;
    response["message"]["state"] = digitalRead(identifier);

    Serial.print("Salio del evento get Status");

  }

  if (eventName == "/change_state") {
    // Los chars que envio por json enrealidad los tengo que recibir como strings
    Serial.print("Entró al evento change State");

    const int identifier = doc["message"]["pin"];
    const int state = doc["message"]["state"];
    Serial.println(identifier);

    changeState(identifier, state);

    response["event_name"] = "/change_state_response";
    response["message"]["pin"] = identifier;
    response["message"]["state"] = digitalRead(identifier);

    Serial.print("Salio del evento change State");

  }

  serializeJson(response, responseBuffer);

  client.publish("aicfe/light_control/esp_1", responseBuffer);
}

void setupWifi(const char* ssid, const char* password) {
  int count = 0;
  bool accessPointMode = false;

  //WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  Serial.printf("Intentando SSID %s y password %s", ssid, password);

  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && !accessPointMode) {
    Serial.printf("Waiting for WiFi... %d segundos", count);
    Serial.println();
    delay(1000);

    count++;

    if (count == 10) {
      accessPointMode = true;
    }
  }

  if (!accessPointMode) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

    // Una vez que sabemos que hay internet, intentamos conectarnos
    connectToMqtt();
  } else {
    setupAccessPoint();
  }
}

void setupAccessPoint() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Configurando (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(APssid, APpassword);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}


void connectToMqtt() {

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  int retryConnections = 0;

  while (!client.connected() && retryConnections != 4) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      retryConnections++;
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  if (retryConnections != 4) {
    Serial.print("Suscribiendome y publicando en topico ");
    Serial.print(topic);

    client.subscribe(topic);
  }

}


void setup() {
  Serial.begin(115200);
  delay(10);

    pinMode(2 , OUTPUT);


  pinMode(LUZ_1 , OUTPUT);
  pinMode(LUZ_2 , OUTPUT);
  pinMode(LUZ_3 , OUTPUT);
  pinMode(LUZ_4 , OUTPUT);
  pinMode(LUZ_5 , OUTPUT);
  pinMode(LUZ_6 , OUTPUT);
  pinMode(LUZ_7 , OUTPUT);
  pinMode(LUZ_8 , OUTPUT);
  pinMode(LUZ_9 , OUTPUT);
  pinMode(LUZ_10 , OUTPUT);
  pinMode(LUZ_11 , OUTPUT);
  pinMode(LUZ_12 , OUTPUT);
  pinMode(LUZ_13 , OUTPUT);
  pinMode(LUZ_14 , OUTPUT);
  pinMode(LUZ_15 , OUTPUT);


  digitalWrite(LUZ_1, LOW);
  digitalWrite(LUZ_2, LOW);
  digitalWrite(LUZ_3, LOW);
  digitalWrite(LUZ_4, LOW);
  digitalWrite(LUZ_5, LOW);
  digitalWrite(LUZ_6, LOW);
  digitalWrite(LUZ_7, LOW);
  digitalWrite(LUZ_8, LOW);
  digitalWrite(LUZ_9, LOW);
  digitalWrite(LUZ_10, LOW);
  digitalWrite(LUZ_11, LOW);
  digitalWrite(LUZ_12, LOW);
  digitalWrite(LUZ_13, LOW);
  digitalWrite(LUZ_14, LOW);
  digitalWrite(LUZ_15, LOW);


  setupWifi(ssid, password);

  // Vamos a intentar conectarnos en un inicio
  connectToMqtt();
}

int contador = 0;

void loop() {
  client.loop();
}
