#ifndef CALLER_ID_H
#define CALLER_ID_H

#include <Arduino.h>

class CallerId
{
public:
    CallerId(){};
    int transmit_caller_id(const char *message, size_t length);

private:
    void send_bytes(char *message, int length);
    void send_mark();
    void send_space();
};

#endif