#include <Arduino.h>
#include <ADCInput.h>
#include "dtmf.h"
#include "config.h"
#include "util.h"
#include "dialtone.h"

Dtmf::Dtmf()
{
}

void Dtmf::begin()
{
    adc.setFrequency(SAMPLE_RATE);
    adc.setBuffers(4, 64);
    adc.begin();
}
char Dtmf::get_dtmf()
{
    int samples[SAMPLE_SIZE];
    float max_x = -1;
    float max_y = -1;
    int n = -1;
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        samples[i] = adc.read();
    }

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

    if (standardDeviation < 30)
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

char Dtmf::dtmf_majority(char n)
{
    // call get_dtmf 3 times and choose the majority result:
    char dtmf[n];
    for (int i = 0; i < n; i++)
    {
        auto res = get_dtmf();
        dtmf[i] = res;
    }
    return findMajority(dtmf, n);
}

int Dtmf::get_number(char *result, size_t size, Dialtone &dialtone)
{
    memset(result, 0, size);
    int i = 0;
    int spccnt = 30;
    static char lastchar;
    while (spccnt)
    {
        char maj = dtmf_majority(5);
        if (!maj)
            continue;
        if (maj == '_')
            spccnt--;
        else
        {
            dialtone.stop();
            if (spccnt > 0 && maj != lastchar)
                result[i++] = maj;
            spccnt = 30;
        }
        lastchar = maj;
        if (i >= size || spccnt == 0)
            return i;
    }
    return 0;
}