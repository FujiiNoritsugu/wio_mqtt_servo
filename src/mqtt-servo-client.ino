#include <Arduino_JSON.h> //https://github.com/arduino-libraries/Arduino_JSON
#include <SPI.h>
#include <AtWiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include "PCA9685.h"  // https://platformio.org/lib/show/6304/Seeed-PCA9685
#include <Wire.h>

const char* WIFI_SSID     = "pr400m-b3354b-1";
const char* WIFI_PASSWORD = "78b2f25d7e38f";

const char* MQTT_SERVER_HOST = "192.168.1.15";
const int   MQTT_SERVER_PORT = 1883;

const char* MQTT_ID       = "id";
const char* MQTT_USERNAME = "name";
const char* MQTT_PASSWORD = "";

const char* OUT_TOPIC = "outTopic";
const char* IN_TOPIC  = "inTopic";

const int REPEAT_INTERVAL = 1000; // [msec.]

static WiFiClient Client;
static PubSubClient Mqtt;
ServoDriver servo;

void setup()
{
  
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  // Servoドライバの初期化
  servo.init(0x7f);

  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Connected.");
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Mqtt.setBufferSize(1024);
  Mqtt.setServer(MQTT_SERVER_HOST, MQTT_SERVER_PORT);
  Mqtt.setCallback(callback);
  Mqtt.setClient(Client);

}

void loop()
{
  if (!Mqtt.connected())
  {
    Serial.print("Connecting to ");
    Serial.println(MQTT_SERVER_HOST);
  
    if (!Mqtt.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("ERROR: Mqtt.connect()");
      goto stop;
    }
    Mqtt.subscribe(IN_TOPIC);
  }

stop:
  unsigned long next = millis();
  while (millis() < next + REPEAT_INTERVAL) Mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Subscribe:");
  for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();
  JSONVar obj = JSON.parse((char *)payload);
  int angle = (int)obj["angle"];
  Serial.println(angle);
  servo.setAngle(1, angle);
  delay(1000);
}
