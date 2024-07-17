#include <Arduino.h>
#include "pbx.h"
PBX pbx = PBX();

void setup()
{
  Serial.begin(9600);
  // while (!Serial)
  //   ;
  Serial.println("Started");
  pbx.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  static int last_blink = 0;
  if (millis() - last_blink > 1000)
  {
    last_blink = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  pbx.handle();
}
