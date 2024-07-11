#ifndef CALLER_ID_H
#define CALLER_ID_H

#include <Arduino.h>
void transmit_caller_id(const char *number);
uint8_t modulo256(char *buffer, int len);
#endif