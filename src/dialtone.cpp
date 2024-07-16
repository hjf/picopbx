#include <Arduino.h>
#include <PWMAudio.h>

#include "dialtone.h"
#include "dialtone_samples.h"
#include "config.h"

PWMAudio pwm(DIALTONE_PIN);

const int16_t *start = (const int16_t *)dialtone_samples;
const int16_t *p = start;

int stop_at = 0;
int count = 0;

void dialtone_cb()
{
  if (stop_at && stop_at < millis())
    return;

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

Dialtone::Dialtone() {}
void Dialtone::start()
{
  if (!started)
  {
    pwm.onTransmit(dialtone_cb);
    started = true;
  }
  pwm.begin(31500);
  stop_at = 0;
}

void Dialtone::start(uint16_t duration)
{
  start();
  stop_at = millis() + duration;
}

void Dialtone::stop()
{
  stop_at = millis();
}