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
    int get_number(char *result, size_t size, Dialtone &dialtone, std::function<bool()> should_continue);

private:
    char get_dtmf();
    char dtmf_majority(char n);

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