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
  pinMode(1, OUTPUT);
}

void loop()
{
  // if (!digitalRead(1))
  // {
  digitalWrite(1, HIGH);
  const char *numbertosend = "2125551212";
  transmit_caller_id(numbertosend);
  digitalWrite(1, LOW);
  delay(1);

  // delay(10);
  //   while (!digitalRead(1))
  //     yield();
  // }
  // handle_pbx();
}
