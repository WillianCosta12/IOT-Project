#include <DHT.h> 
#include <FS.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "SPIFFS.h"

#define DHTTYPE DHT22

#define IO_USERNAME  "Willianc12"
#define IO_KEY       "aio_FtGU81raG43k8Ya40dTlCGE3e1wv" 
const char* ssid = "NPITI-IoT";
const char* password = "NPITI-IoT";

const char* mqttserver = "io.adafruit.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

DHT dht(4, DHT22);
int count = 1;

int num=0;
String str;
String s;

void setup_wifi() {

  delay(10);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Se conectado
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("conectado");
      // Depois de conectado, publique um anúncio ...
      client.publish("Willianc12/feeds/temperatura", "Iniciando Comunicação");
      //... e subscribe.
      client.subscribe("Willianc12/feeds/temperatura");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}


void writeFile(String state, String path) { //escreve conteúdo em um arquivo
  File rFile = SPIFFS.open(path, "a");//a para anexar
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("tamanho");
    Serial.println(rFile.size());
    rFile.println(state);
    Serial.print("Gravou: ");
    Serial.println(state);
  }
  rFile.close();
}

String readFile(String path) {
  Serial.println("Read file");
  File rFile = SPIFFS.open(path, "r");//r+ leitura e escrita
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("----------Lendo arquivo ");
    Serial.print(path);
    Serial.println("  ---------");
    while (rFile.position() < rFile.size())
    {
      s = rFile.readStringUntil('\n');
      s.trim();
      Serial.println(s);
    }
    rFile.close();
    return s;
  }
}

void formatFile() {
  Serial.println("Formantando SPIFFS");
  SPIFFS.format();
  Serial.println("Formatou SPIFFS");
}

void openFS(void) {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void setup() {
  Serial.begin(115200);

  //Serial.println("abrir arquivo");
  //openFS(); 

  dht.begin();

  setup_wifi();
  client.setServer(mqttserver, 1883);

}

void loop() {
  
  if (!client.connected()) {
    //String test = readFile("/logsAula.txt");
    //Serial.println(test);

    reconnect();
  }
  client.loop();

  delay(5000); 

  float t = dht.readTemperature();

  char s_temp[8];
  dtostrf(t,1,2,s_temp);

  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Temperatura, Amostra "));
  Serial.print(count);
  Serial.print(F(": "));
  Serial.print(t);
  Serial.println(F("°C "));

  //if (!client.connected() || t > 30) {
    //str = "Temperatura, Amostra " + String(count) + " : " + String(t) + "°C ";
    //writeFile(str , "/logsAula.txt");
  //}

  client.publish("Willianc12/feeds/temperatura", s_temp);

  count++;

}