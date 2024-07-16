#ifndef CALLER_ID_H
#define CALLER_ID_H

#include <Arduino.h>

class CallerId {
    public:
        CallerId();
        ~CallerId();
        int transmit_caller_id(const char *message, size_t length);
        uint8_t modulo256(char *buffer, int len);
    private:
        void send_bytes(char *message, int length);
        void send_mark();
        void send_space();
};

#endif