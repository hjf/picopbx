#include <Arduino.h>
#include "pbx.h"
#include "dtmf.h"
#include "callerid.h"
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");
  initialize_pbx();
  dtmf_setup();
  pinMode(1, INPUT_PULLUP);
}

void loop()
{
  handle_pbx();
}
