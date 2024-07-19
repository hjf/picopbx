#include <Arduino.h>
#include <PWMAudio.h>

#include "dialtone.h"
#include "dialtone_samples.h"
#include "config.h"

PWMAudio pwm(DIALTONE_PIN);

volatile int Dialtone::stop_at = 0;
volatile int Dialtone::count = 0;

void Dialtone::dialtone_cb()
{
  static const int16_t *start = (const int16_t *)dialtone_samples;
  static const int16_t *p = start;
  if (stop_at && stop_at < millis())
  {
    pwm.end();
    pinMode(DIALTONE_PIN, INPUT);
    return;
  }

  while (pwm.availableForWrite())
  {
    pwm.write(*p++ * 2);
    count += 2;
    if (count >= sizeof(dialtone_samples))
    {
      count = 0;
      p = start;
    }
  }
}

void Dialtone::start()
{
  if (!started)
  {
    pwm.onTransmit(Dialtone::dialtone_cb);
    started = true;
  }
  pwm.begin(31500);
  stop_at = 0;
}

void Dialtone::start(uint16_t duration)
{
  pinMode(DIALTONE_PIN, OUTPUT);
  start();
  stop_at = millis() + duration;
}

void Dialtone::stop()
{
  stop_at = millis();
}