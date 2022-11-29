//#include <WiFi.h>
#include <ESP8266WiFi.h>

#include <WebSocketsServer.h>
#include <string.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define MQTTpubQos 2

/* --- TECLAS --- */

//
#define teclaLuzCocina 22

#define teclaLuzComedor 23

/* --- FIN TECLAS --- */


/* --- RELES --- */

//
#define releLuzCocina 13

#define releLuzComedor 22

#define dimmerLuzComedor 23

/* --- FIN RELES --- */

WiFiClient espClient;
PubSubClient client(espClient);
WebSocketsServer webSocket = WebSocketsServer(81);

// Esto debe guardarse en la memoria EEPROM para restaurar internet en caso de corte de luz
const char* ssid = "ZAMUDIO_2.4";
const char* password = "zamudio%";

// MQTT Broker
const char *mqtt_broker = "192.168.0.62";
const char *topic = "kevinfigueroadavid2000@gmail.com/5eb03852-d4e5-4407-aa6b-c84f388939af";
const char *mqtt_username = "esp32/test";
const char *mqtt_password = "public";
const char *will_topic = "";
const int mqtt_port = 1883;

// Esto debe tener un id Unico para identificar como red unica
const char* APssid = "Dómotica casa walter";
const char* APpassword = "12345678";

int beforeStateTeclaLuzCocina = 0;
int beforeStateTeclaLuzComedor = 0;

/*
  TODO:

  ACA ME DEBE LLEGAR EN REALIDAD EL NUMERO DE PIN QUE DEBO MODIFICAR, ENTONCES NO TENGO QUE TENER IDENTIFICADORES,
  SI NO QUE MODIFICO DIRECTAMENTE EL INT DEL PIN QUE ME LLEGA DESDE MQTT

  DE ESTA MANERA TAMBIEN SE VA A FACILITAR A LA HORA DE ENVIAR DESDE EL ESP A MQTT EL VALOR DEL PIN DEL OUTPUT DE LA TECLA QUE SE MODIFICÓ ( COMUNICACIÓN BIDIRECCIONAL)

*/


void changeState(String identifier) {
  if (identifier == "cocina_luz_1")
  {
    
    digitalWrite(releLuzCocina, !digitalRead(releLuzCocina));
    Serial.println("Entré a cambiar el estado de cocina luz 1");

  }

  if (identifier == "comedor_luz_2")
  {
    Serial.println("Entré a cambiar el estado de cocina luz 2");
    digitalWrite(releLuzComedor, !digitalRead(releLuzComedor));
  }

  if (identifier == "comedor_dimmer_luz_1")
  {
    Serial.println("Entré a cambiar el estado de comedor dimmer luz 1");
    digitalWrite(dimmerLuzComedor, !digitalRead(dimmerLuzComedor));
  }

  return;
}

void decodeMessage(uint8_t num, char* json) {
  // Leer Json
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  // Responder Json
  DynamicJsonDocument response(1024);
  char responseBuffer[100];

  String eventName = doc["event_name"];
  Serial.print(eventName);


  /*
    // Si el tipo de dispositivo es "luz"
    if(eventName == "/change_state"){
     String state = doc["message"]["state"];

     if(state == "on"){
        digitalWrite(2, HIGH);
      } else if(state == "off"){
        digitalWrite(2, LOW);
      }
    }

    if(eventName == "/set_information"){

      ssid = doc["message"]["ssid"];
      password = doc["message"]["password"];

      topic = doc["message"]["topic_mqtt_device"];
      user = doc["message"]["user"];
      room = doc["message"]["room"];

      // Este es el ultimo evento de la configuracion de dispositivo desde la APP, para este entonces, ya tendré cargado los datos del ssid y password en la placa y solo debo conectarme

      // TODO: ARREGLAR EL POR QUE ME ESTA MOSTRANDO PARTE DE OTRAS VARIABLES CUANDO QUIERO PASARLE EL SSID Y PASSWORD PARA CONFIGURAR WIFI
       setupWifi(ssid, password);

      // TODO: UNA VEZ QUE ME CONECTÉ A INTERNET, NECESITO SUSCRIBIRME AL TOPICO QUE ME DA LA APP Y GUARDAR ESE VALOR EN LA MEMORIA EEPROM IGUAL QUE LA PASSWORD Y SSID DEL WIFI
      response["event_name"] = "/set_information_response";

    }

    if(eventName == "/device_information"){

      // TODO: Necesito darle Feedback a la app de que se recibió este evento
      response["event_name"] = "/device_information_response";
      response["type_of_device"] = typeOfDevice;

      //TODO: enviar por WS este response
    }

  */

  serializeJson(response, responseBuffer);
  // ENVIAMOS RESPONSE GENERADO A LA APP
  webSocket.sendTXT(num, responseBuffer);
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
    // Este evento no nos interesa
    return;
  }

  if (eventName == "/finish_configuration") {
    // Este evento no nos interesa
    return;
  }

  if (eventName == "/change_state") {
    // Los chars que envio por json enrealidad los tengo que recibir como strings

    //changeState(doc["message"]["identifier_in_controller"]);
    String identifier = doc["message"]["identifier_in_controller"];
    const int state = doc["message"]["state"];
    Serial.println(identifier);

    changeState(identifier);

    response["event_name"] = "/change_state_response";
    response["message"]["identifier_in_controller"] = identifier;
    response["message"]["state"] = state;

  }

  serializeJson(response, responseBuffer);

  client.publish("kevinfigueroadavid2000@gmail.com/5eb03852-d4e5-4407-aa6b-c84f388939af", responseBuffer);

  //Serial.println();
  //Serial.println("-----------------------");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      char* json = (char*)payload;

      // Decodificamos el mensaje desde WS
      decodeMessage(num, json);
      break;
  }
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

    // Responder Json
    //DynamicJsonDocument response(1024);
    //char responseBuffer[250];

    //response["user"] = user;
    //response["device"]["room"] = room;
    //response["device"]["type_of_device"] = typeOfDevice;
    //response["device"]["topic_mqtt_device"] = topic;

    //serializeJson(response, responseBuffer);

    // TODO: será que no puedo enviar el mensaje desde el esp porque es muy largo el topico??

    // Este evento lo va a recibir el backend y va a generar una bandera de este dispositivo para luego avisar a la app
    //client.publish(configurationDevice, responseBuffer, MQTTpubQos);
    client.subscribe(topic);
  }

}



void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(teclaLuzCocina , INPUT_PULLDOWN);
  pinMode(teclaLuzComedor , INPUT_PULLDOWN);

  pinMode(releLuzCocina , OUTPUT);
  //pinMode(releLuzComedor , OUTPUT);
  //pinMode(dimmerLuzComedor , OUTPUT);

  setupWifi(ssid, password);

  // Vamos a intentar conectarnos en un inicio
  connectToMqtt();

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}

      int contador = 0;
      
void loop() {

  // Tecla luz cocina


  int teclaLuzCocinaValue = digitalRead(teclaLuzCocina);

    //if(teclaLuzCocinaValue == 0){
      
    //  contador += 1;
    //}

    //if

    //delay(20);



  /*
    // Tecla luz comedor

    int teclaLuzComedorValue = digitalRead(teclaLuzComedor);

    if(teclaLuzComedorValue != beforeStateTeclaLuzComedor) {
      beforeStateTeclaLuzComedor = digitalRead(teclaLuzComedor);

      if(teclaLuzComedorValue == 0){
          digitalWrite(releLuzComedor, LOW);
       } else {
          digitalWrite(releLuzComedor, HIGH);
       }
    }

  */

  client.loop();
  webSocket.loop();

}
