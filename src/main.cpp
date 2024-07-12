#include <Arduino.h>
#include "pbx.h"
#include "dtmf.h"
#include "bell202.h"
#include "callerid.h"
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");
  initialize_pbx();
  dtmf_setup();
  bell202_begin();

  pinMode(1, INPUT_PULLUP);
}

void loop()
{
  if (!digitalRead(1))
  {
    const char *numbertosend = "2125551212";
    transmit_caller_id(numbertosend);
    delay(10);
    while (!digitalRead(1))
      yield();
  }
  // handle_pbx();
}
