#ifndef DTMF_H
#define DTMF_H

#include <Arduino.h>
#define ADC_MIDPOINT (2048)
#define SAMPLE_RATE (8192)
#define SAMPLE_SIZE (100)
#define MAG_THRESHOLD (5000)

void dtmf_setup();

int get_number(char *result, size_t size);
#endif