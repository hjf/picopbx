#include <Arduino.h>
#include "pbx.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Started");
  initialize_pbx();
}

void loop()
{
  handle_pbx();
}
