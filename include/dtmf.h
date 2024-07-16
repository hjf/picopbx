#ifndef DTMF_H
#define DTMF_H

#include <Arduino.h>
#include "dialtone.h"
#include "Goertzel.h"
#include <ADCInput.h>

#define ADC_MIDPOINT (2048)
#define SAMPLE_RATE (8192)
#define SAMPLE_SIZE (100)
#define MAG_THRESHOLD (5000)

class Dtmf
{
public:
    Dtmf();
    void begin();
    int get_number(char *result, size_t size, Dialtone &dialtone);

private:
    char get_dtmf();
    char dtmf_majority(char n);
    Goertzel X0 = Goertzel(1209.0, SAMPLE_RATE);
    Goertzel X1 = Goertzel(1336.0, SAMPLE_RATE);
    Goertzel X2 = Goertzel(1477.0, SAMPLE_RATE);
    Goertzel Y0 = Goertzel(697.0, SAMPLE_RATE);
    Goertzel Y1 = Goertzel(770.0, SAMPLE_RATE);
    Goertzel Y2 = Goertzel(852.0, SAMPLE_RATE);
    Goertzel Y3 = Goertzel(941.0, SAMPLE_RATE);
    ADCInput adc = ADCInput(A0);

    char dtmf_lut[16] = {
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
        0,
        0,
        0,
        0,
    };
};
#endif