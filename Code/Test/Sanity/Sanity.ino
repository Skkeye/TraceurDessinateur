#include <Arduino.h>



void setup()
{
  pinMode(1, OUTPUT);
}

void loop()
{
	digitalWrite(1, HIGH);
  delay(200);
	digitalWrite(1, LOW);
  delay(200);
}
