#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//  THINGSBOARD 
#define TOKEN "aUC70DN4wd3Li1WUUwFs"
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_PORT 1883

// WIFI 
const char* ssid = "OLP";
const char* password = "oteitzaLP";

// PINAK
const int ldrPin = A0;
const int ledPin = D7;

// ALDAGAIAK
int ldrValue = 0;

int thresholdOn  = 1000;
int thresholdOff = 1010;

bool ledState = false;

// 
WiFiClient espClient;
PubSubClient client(espClient);

// WIFI KONEXIOA
void conectarWiFi() {

  Serial.print("WIFI-ra konektatzen");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Konektatuta");
}

// THINGSBOARD KONEXIOA
void reconnect() {

  while (!client.connected()) {

    Serial.print("Thingsboarden konektatzen...");

    if (client.connect("NodeMCU", TOKEN, NULL)) {
      Serial.println("Konektatuta");
    } 
    else {
      Serial.print("Errorea ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {

  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  conectarWiFi();

  client.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // LDR IRAKURRI
  ldrValue = analogRead(ldrPin);

  // PIZTU LED
  if (ldrValue < thresholdOn) {
    ledState = true;
  }

  // ITZALI LED
  if (ldrValue > thresholdOff) {
    ledState = false;
  }

  digitalWrite(ledPin, ledState);

  // JSON SORTU
  String payload = "{";
  payload += "\"LDR\":";
  payload += ldrValue;
  payload += ",";
  payload += "\"LED\":";
  payload += ledState;
  payload += "}";

  // THINGSBOARD-era BIDALI
  client.publish("v1/devices/me/telemetry", payload.c_str());

  Serial.println(payload);

 delay(200);
}
