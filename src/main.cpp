#include <Arduino.h>
// #include "pbx.h"
// #include "dtmf.h"
// #include "callerid.h"
#include "ringer.h"
#include "config.h"
Ringer mringer = Ringer(RINGER_Q1, RINGER_Q2, RINGER_Q3, RINGER_RELAY, RINGER_FREQUENCY);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Started");
  // initialize_pbx();
  // dtmf_setup();
  // pinMode(1, INPUT_PULLUP);
}

void loop()
{
  static unsigned long last = 0;
  // handle_pbx();
  mringer.handle();

  if (millis() - last > 2000)
  {
    last = millis();
    mringer.start(1000, []()
                  { Serial.println("Ring ended"); });
  }
}
