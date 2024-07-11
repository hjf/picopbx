#ifndef PBX_H
#define PBX_H

#include <Arduino.h>

typedef enum
{
    UNINITIALIZED,
    IDLE,
    BUSY,
    WAIT_FOR_DIAL,
    CALLING,
    CONNECTED
} State;

String state_to_string(State s);

void handle_pbx();
void initialize_pbx();

#endif