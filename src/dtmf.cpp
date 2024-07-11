#include <Arduino.h>
#include "dtmf.h"
#include "config.h"
#include "util.h"
#include <Goertzel.h>

Goertzel X0(1209.0, SAMPLE_RATE);
Goertzel X1(1336.0, SAMPLE_RATE);
Goertzel X2(1477.0, SAMPLE_RATE);
Goertzel Y0(697.0, SAMPLE_RATE);
Goertzel Y1(770.0, SAMPLE_RATE);
Goertzel Y2(852.0, SAMPLE_RATE);
Goertzel Y3(941.0, SAMPLE_RATE);

char dtmf_lut[] = {
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '*',
    '0',
    '#',
    'p',
    'q',
    'r',
    's',
};

char get_dtmf()
{
    int samples[SAMPLE_SIZE];
    float max_x = -1;
    float max_y = -1;
    int n = -1;
    noInterrupts();
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        samples[i] = analogRead(A0);
    }
    interrupts();

    // calculate standard deviation from samples:
    float sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        sum += samples[i];
    }
    float mean = sum / SAMPLE_SIZE;

    float variance = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        variance += pow(samples[i] - mean, 2);
    }
    float standardDeviation = sqrt(variance / SAMPLE_SIZE);

    if (standardDeviation < 10)
    {
        return '_';
    }

    n = 0;
    float current_x = X0.Mag(samples, SAMPLE_SIZE);
    max_x = current_x;

    current_x = X1.Mag(samples, SAMPLE_SIZE);
    if (current_x > max_x)
    {
        n = 1;
        max_x = current_x;
    }
    current_x = X2.Mag(samples, SAMPLE_SIZE);
    if (current_x > max_x)
    {
        n = 2;
        max_x = current_x;
    }

    if (max_x < MAG_THRESHOLD)
        return 0;

    float current_y = Y0.Mag(samples, SAMPLE_SIZE);
    max_y = current_y;
    int ysum = 0;
    current_y = Y1.Mag(samples, SAMPLE_SIZE);

    if (current_y > max_y)
    {
        ysum = 3;
        max_y = current_y;
    }

    current_y = Y2.Mag(samples, SAMPLE_SIZE);
    if (current_y > max_y)
    {
        ysum = 6;
        max_y = current_y;
    }

    current_y = Y3.Mag(samples, SAMPLE_SIZE);
    if (current_y > max_y)
    {
        ysum = 9;
        max_y = current_y;
    }

    if (max_y < MAG_THRESHOLD)
        return 0;

    n += ysum;

    return dtmf_lut[n & 0x0f];
}

char dtmf_majority(char n)
{
    // call get_dtmf 3 times and choose the majority result:
    char dtmf[n];
    for (int i = 0; i < n; i++)
    {
        auto res = get_dtmf();
        if (res > 0 && res != '_')
            digitalWrite(LED_BUILTIN, LOW);
        dtmf[i] = res;
    }
    return findMajority(dtmf, n);
}
