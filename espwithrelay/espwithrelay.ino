#include <WiFi.h>
#include <PubSubClient.h>

// Define your WiFi credentials
const char* ssid = "OPPO A16"; // name sa wifi
const char* password = "123456789"; // password

// Define your MQTT broker details
const char* mqtt_server = "192.168.215.178";// adress sa server

const int BUTTON_PIN_1 = 12; // Change these pins according to your wiring
const int BUTTON_PIN_2 = 13;
const int BUTTON_PIN_3 = 14;
const int motorPin = 36;  // ADC input pin on Wemos D1 R32
float voltage = 0.0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle MQTT messages if needed
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(motorPin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   analogReadResolution(12);  // Set ADC resolution to 12 bits
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read button states and publish to MQTT
  int buttonState1 = digitalRead(BUTTON_PIN_1);
  int buttonState2 = digitalRead(BUTTON_PIN_2);
  int buttonState3 = digitalRead(BUTTON_PIN_3);
  
  int motorValue = analogRead(motorPin);
  voltage = motorValue * (100 / 4095.0)-71;   // Convert ADC value to voltage (assuming 12-bit ADC)
  Serial.println(voltage);
  
  client.publish("button/1", String(buttonState1).c_str());
  client.publish("button/2", String(buttonState2).c_str());
  client.publish("button/3", String(buttonState3).c_str());
  client.publish("voltage1", String(voltage).c_str());

  delay(100); // Adjust delay as needed
}
