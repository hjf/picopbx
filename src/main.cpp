#include <Arduino.h>
#include "pbx.h"
PBX pbx = PBX();

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");
  pbx.begin();
}

void loop()
{
  pbx.handle();
}
