/*
  58. rész - Automata öntözőrendszer

  Magyar Arduino Labor, 2018.06.12.
*/

#include <ESP8266WiFi.h>

#define WLAN_SSID   "router nev"
#define WLAN_PASS   "jelszo"

const int szenzor = A0;
int szenzorertek = 0;
int szazalekertek = 0;

const int motor1 = 5;   //D1
const int motor2 = 4;   //D2

const int tranzisztor = 0; //D3

const int vizszintMero = 2; //D4
int vizszint;
const int led = 14; //D5
const int tranzisztor2 = 12//D6

void setup()
{
  Serial.begin(115200);

  Serial.print("Csatlakozás ide: ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi sikeresen csatlakoztatva");
  Serial.println("IP cím: "); Serial.println(WiFi.localIP());

  pinMode(motor1, OUTPUT);
  digitalWrite(motor1, LOW);
  pinMode(motor2, OUTPUT);
  digitalWrite(motor2, LOW);

  pinMode(tranzisztor, OUTPUT);
  digitalWrite(tranzisztor, LOW);

  pinMode(tranzisztor2, OUTPUT);
  digitalWrite(tranzisztor2, LOW);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop()
{
  digitalWrite(tranzisztor, HIGH);
  delay(200);
  szenzorertek = analogRead(szenzor);
  szazalekertek = map(szenzorertek, 1023, 300, 0, 100);
  delay(200);
  digitalWrite(tranzisztor, LOW);

  Serial.print("Szenzor erteke: ");
  Serial.println(szenzorertek);
  Serial.print("Szazalekban: ");
  Serial.print(szazalekertek);
  Serial.println("%");

  if (szazalekertek < 50)
  {
    motor_control();
  }

  
  digitalWrite(tranzisztor2, HIGH);
  delay(100);
  vizszint = digitalRead(vizszintMero);
  delay(100);
  digitalWrite(tranzisztor2, LOW);

  if (vizszint == HIGH)
  {
    digitalWrite(led, HIGH);
    delay(3000);
    digitalWrite(led, LOW);
  }
  else
  {
    Serial.println("Vizszint rendben");
  }

  ESP.deepSleep(1 * 60 * 60 * 1000 * 1000); //1 óra deep sleep

}


void motor_control()
{
  digitalWrite(motor1, HIGH);
  digitalWrite(motor2, LOW);
  delay(10000);
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
}

