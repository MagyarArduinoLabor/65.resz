#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// Replace with your network credentials
#define WLAN_SSID   "routernev"
#define WLAN_PASS   "jelszo"

//MQTT broker settings
#define HOST        "192.168.x.x"
#define PORT        1883
#define USERNAME    "user"
#define PASSWORD    "password"

#define DHTPIN D2
DHT dht(DHTPIN, DHT22);

float DHT_temperature = 0.0;

uint8_t retries = 5;
uint8_t mqtt_retries = 3;
const uint16_t intervall = 60 * 60 * 1000 * 1000;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "iot_weather_station/temperature");

void MQTT_connect();

void setup()
{
  /*WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);*/
  Serial.begin(115200);

  /*
    Routerhez való csatlakozás
  */
  Serial.print("Csatlakozás ");
  Serial.println(WLAN_SSID);
  Serial.println("-hoz");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi csatlakoztatva");
  Serial.println("IP cim: "); Serial.println(WiFi.localIP());

  /*
    A DHT22 szenzor inicializálása
  */
  Serial.println("DHT22 inicializálása");
  dht.begin();

  delay(500);

  /*
    MQTT szerverhez való csatlakozás
  */
  MQTT_connect();

  /*
     Mérés
  */
  DHT_read();

  /*
    Publikálás
  */
  MQTT_publish();

  /*WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();*/
  ESP.deepSleep(intervall);
}

void loop()
{

}

void MQTT_connect()
{
  int8_t ret;

  Serial.print("Csatlakozas az MQTT szerverhez... ");
  if (mqtt.connected())
  {
    return;
  }

  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Ujraprobalkozas 5 masodpercen belul...");
    mqtt.disconnect();
    delay(5000);
    mqtt_retries--;
    if (mqtt_retries == 0)
    {
      Serial.println("MQTT csatlakozas sikertelen!");
      ESP.deepSleep(intervall);
    }
  }
  Serial.println("MQTT csatlakozas sikeres!");
}

void DHT_read()
{
  while (isnan(DHT_temperature) || DHT_temperature == 0)
  {
    DHT_temperature = dht.readTemperature();
    delay(2000);

    Serial.print("Homerseklet = ");
    Serial.print(DHT_temperature);
    Serial.println(" Celsius");

    retries--;
    if (retries == 0)
    {
      return;
    }
  }
}

void MQTT_publish()
{
  Serial.println("Publikalas...");
  temperature.publish(DHT_temperature);
  delay(500);
  Serial.println("Publikalas kész!");
}
