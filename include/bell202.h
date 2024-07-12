#ifndef BELL202_H
#define BELL202_H
#include <Arduino.h>
int bell202_send(const char *msg, size_t len, bool blocking);
void bell202_begin();
void bell202_stop();
void bell202_tone(bool mark, int duration);
#endif
