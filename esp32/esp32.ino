#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>

#define WIFI_SSID "Nome da Rede Wifi"
#define WIFI_PASSWORD "Senha da Rede Wifi"

#define DHT_PIN 2
#define DHT_TYPE DHT22

#define THINGSPEAK_API_KEY "API KEY DO ThingSpeak"
#define THINGSPEAK_HOST "api.thingspeak.com"

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Conectando a rede Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando na rede...");
  }
  Serial.println("Connectado!");

  dht.begin();
}

void loop() {
  delay(2000);

  // Leitura dos valores de temperatura e umidade
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Falha em ler pelo sensor DHT!");
    return;
  }

  // Estabelecendo conexão com o ThingSpeak
  if (client.connect(THINGSPEAK_HOST, 80)) {
    String data = "field1=" + String(temperature) + "&field2=" + String(humidity);
    String request = "POST /update HTTP/1.1\n" +
                     "Host: " + String(THINGSPEAK_HOST) + "\n" +
                     "Connection: close\n" +
                     "X-THINGSPEAKAPIKEY: " + String(THINGSPEAK_API_KEY) + "\n" +
                     "Content-Type: application/x-www-form-urlencoded\n" +
                     "Content-Length: " + String(data.length()) + "\n\n" +
                     data;

    client.print(request);
    Serial.println("Enviando dados para o ThingSpeak...");

    // Verificando a resposta do servidor
    String response = client.readStringUntil('\r');
    Serial.println(response);

    client.stop();
  }
}