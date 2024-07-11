#ifndef DTMF_H
#define DTMF_H

#include <Arduino.h>
#define ADC_MIDPOINT (512)
#define SAMPLE_RATE (8900)
#define SAMPLE_SIZE (50)
#define MAG_THRESHOLD (80)

char get_dtmf();
char dtmf_majority(char n);
#endif