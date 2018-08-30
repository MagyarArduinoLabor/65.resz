/******************************************************************************
Fotorezisztor olvasás

Software author: Kotan Tamas Balazs
E-mail: magyar.arduino.labor@gmail.com
******************************************************************************/

const int sensorPin = A0;
int sensorValue;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue); 
  delay(1000);
}
