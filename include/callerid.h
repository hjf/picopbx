#ifndef CALLER_ID_H
#define CALLER_ID_H

#include <Arduino.h>

typedef struct __attribute__((__packed__))
{
    char datetime[8];
    char number[32];
} CalledNumber;

class CallerId
{
public:
    CallerId(){};
    int transmit_caller_id(const CalledNumber *number, int length);

private:
    void send_bytes(char *message, int length);
    void send_mark();
    void send_space();
};

#endif