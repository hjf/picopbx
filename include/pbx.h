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
    static bool is_caller_off_hook();

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
    CalledNumber called_number;
    static void caller_hook_isr();
    static void dest_hook_isr();
    static volatile boolean caller_hook_read;
    static volatile boolean dest_hook_read;
    static volatile unsigned long caller_hook_last_transition;
    static volatile unsigned long dest_hook_last_transition;
    static volatile bool caller_off_hook;
    static volatile bool dest_off_hook;
};
#endif