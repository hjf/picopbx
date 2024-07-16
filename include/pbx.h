#ifndef PBX_H
#define PBX_H

#include <Arduino.h>
#include "ringer.h"
#include "pbx.h"
#include "config.h"
#include "dialtone.h"
#include "callerid.h"
#include "dtmf.h"
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
    ~PBX();
    void handle();
    void begin();
    static String state_to_string(State s);

private:
    Ringer ringer;
    CallerId callerId;
    Dialtone dialtone;
    Dtmf dtmf;
    State state = UNINITIALIZED;
    State last_state = UNINITIALIZED;
    unsigned long last_state_change = 0;
    unsigned long last_tone_change = 0;
    long rings = 0;
    char called_number[64] = {'0', '1', '0', '1', '0', '0', '0', '0'};
    static void caller_hook_isr();
    static void dest_hook_isr();
    static volatile boolean caller_hook_read;
    static volatile boolean dest_hook_read;
    static unsigned long caller_hook_last_transition;
    static unsigned long dest_hook_last_transition;
    static bool caller_off_hook;
    static bool dest_off_hook;
};
#endif