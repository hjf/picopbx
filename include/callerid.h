#ifndef CALLER_ID_H
#define CALLER_ID_H

#include <Arduino.h>
int transmit_caller_id(const char *message, size_t length);
uint8_t modulo256(char *buffer, int len);
#endif