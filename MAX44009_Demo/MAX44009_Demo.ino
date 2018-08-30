#include "MAX44009.h"

MAX44009 Lux(0x4A);

void setup() {
  Serial.begin(9600);
  
  Lux.Begin(0, 188000); //Begin with full range min and max values
}

void loop() {
  Serial.print("Brightness = ");
  Serial.print(Lux.GetLux());
  Serial.println(" Lux \t");
  
  delay(1000);

}


