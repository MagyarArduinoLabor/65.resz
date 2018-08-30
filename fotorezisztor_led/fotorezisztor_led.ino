/******************************************************************************
Fotorezisztor_led

Software author: Kotan Tamas Balazs
E-mail: magyar.arduino.labor@gmail.com
******************************************************************************/
const int sensorPin = A0;
const int led = 11;
int sensorValue = 0;
int ledValue = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);

  ledValue = map(sensorValue, 150, 1023, 255, 0);
  Serial.println(ledValue);

  analogWrite(led, ledValue);
}
