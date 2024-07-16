#ifndef PBX_H
#define PBX_H

#include <Arduino.h>
#include "ringer.h"

typedef enum
{
    UNINITIALIZED,
    IDLE,
    BUSY,
    WAIT_FOR_DIAL,
    CALLING,
    CONNECTED
} State;

class PBX
{
public:
    PBX();
    void handle();
    void begin();
    static String state_to_string(State s);

private:
    Ringer ringer;
    State state = UNINITIALIZED;
    State last_state = UNINITIALIZED;
    unsigned long last_state_change = 0;
    unsigned long last_tone_change = 0;
    long rings = 0;
    char called_number[64] = {'0', '1', '0', '1', '0', '0', '0', '0'};
};
#endif