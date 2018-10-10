#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "DHT.h"
#include <Adafruit_BMP085.h>
#include <Wire.h>
#include "MAX44009.h"

// Replace with your network credentials
#define WLAN_SSID   "routernev"
#define WLAN_PASS   "jelszo"

//MQTT broker settings
#define HOST        "192.168.x.x"
#define PORT        1883
#define USERNAME    "user"
#define PASSWORD    "password"

#define DHTPIN D2

#define scl D3
#define sda D4
Adafruit_BMP085 bmp180;

DHT dht(DHTPIN, DHT22);
MAX44009 Lux(0x4A);

float DHT_temperature = 0.0;
float DHT_humidity = 0.0;
float BMP_pressure = 0.0;
float MAX_light = 0.0;

uint8_t retries = 5;
uint8_t mqtt_retries = 3;
const uint16_t intervall = 60 * 60;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "iot_weather_station/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "iot_weather_station/humidity");
Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, "iot_weather_station/pressure");
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "iot_weather_station/light");
void MQTT_connect();


void setup()
{
  Serial.begin(115200);

  /*
    Routerhez való csatlakozás
  */
  Serial.print("Csatlakozás ");
  Serial.print(WLAN_SSID);
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

  Wire.begin(sda, scl);

  /*
    A DHT22 szenzor inicializálása
  */
  Serial.println("DHT22 inicializálása");
  dht.begin();

  /*
    A MAX44009 szenzor inicializálása
  */
  Serial.println("MAX44009 inicializálása");
  Lux.Begin(0, 188000);

  /*
    A BMP180 szenzor inicializálása
  */
  Serial.println("BMP180 inicializálása");
  if (bmp180.begin())
  {
    Serial.println("Sikeres inicializálás!");
  }
  else
  {
    Serial.println("Sikertelen inicializálás!");
    BMP_pressure = 0;
  }

  delay(500);

  /*
    MQTT szerverhez való csatlakozás
  */
  MQTT_connect();

  /*
    Mérések
  */
  DHT_read();
  BMP_read();
  MAX_read();

  /*
    Publikálás
  */
  MQTT_publish();

  /*
    Belépés deep sleep módba
  */
  Serial.println("Deep sleep");
  ESP.deepSleep(intervall * 1000 * 1000);
}

void loop()
{

}

void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Csatlakozas az MQTT szerverhez... ");
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
      ESP.deepSleep(60 * 60 * 1000 * 1000);
    }
  }
  Serial.println("MQTT csatlakozas sikeres!");
}


void DHT_read()
{
  while (isnan(DHT_temperature) || DHT_temperature == 0  || isnan(DHT_humidity) || DHT_humidity == 0)
  {
    DHT_temperature = dht.readTemperature();
    delay(1000);
    DHT_humidity = dht.readHumidity();
    delay(1000);

    Serial.print("Homerseklet = ");
    Serial.print(DHT_temperature);
    Serial.println(" Celsius");
    Serial.print("Paratartalom = ");
    Serial.print(DHT_humidity);
    Serial.println(" %");

    retries--;
    if (retries == 0)
    {
      return;
    }
  }
}

void BMP_read()
{
  BMP_pressure = bmp180.readPressure();
  Serial.print("Legnyomas = ");
  Serial.print(BMP_pressure);
  Serial.println(" Pascal");
  Serial.println();
}

void MAX_read()
{
  MAX_light = Lux.GetLux();
  Serial.print("Fenyerosseg = ");
  Serial.print(MAX_light);
  Serial.println(" Lux");
}

void MQTT_publish()
{
  Serial.println("Publikalas...");
  temperature.publish(DHT_temperature);
  delay(500);
  humidity.publish(DHT_humidity);
  delay(500);
  pressure.publish(BMP_pressure);
  delay(500);
  light.publish(MAX_light);
  delay(500);
  Serial.println("Publikalas kész!");
}



