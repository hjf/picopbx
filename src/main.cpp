#include <Arduino.h>
#include "pbx.h"
#include "dtmf.h"
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");
  initialize_pbx();
  dtmf_setup();
}

void loop()
{
  handle_pbx();
}
