#include <Arduino.h>
#include "ringer.h"

Ringer::Ringer(int q1, int q2, int q3, int q4, int relay, int freq)
{
    _q1 = q1;
    _q2 = q2;
    _q3 = q3;
    _q4 = q4;
    _relay = relay;
    period = 1000 / freq;
    digitalWrite(_relay, 0);
    pinMode(_relay, OUTPUT);
}

void Ringer::handle()
{
    if (millis() - last_change > period)
    {
        forward = !forward;
        if (forward)
            set_forward();
        else
            set_reverse();
    }
    if (stop_at && millis() > stop_at)
    {
        stop();
        if (timeout_callback)
            timeout_callback();
    }
}

void Ringer::start(int duration, void (*timeout_callback)(void))
{
    this->timeout_callback = timeout_callback;
    stop_at = millis() + duration;
    digitalWrite(_relay, HIGH);
    start();
}

void Ringer::start()
{
    stop_at = 0;
    forward = true;
    running = true;
    last_change = 0;
    ring_counter = 0;
}

void Ringer::stop()
{
    set_coasting();
    digitalWrite(_relay, LOW);
    running = false;
}

void Ringer::set_forward()
{
    pinMode(_q1, INPUT);
    pinMode(_q2, INPUT);
    pinMode(_q3, INPUT);
    pinMode(_q4, INPUT);
    digitalWrite(_q2, 0);
    digitalWrite(_q3, 1);
    pinMode(_q2, OUTPUT);
    pinMode(_q3, OUTPUT);
}

void Ringer::set_reverse()
{
    pinMode(_q1, INPUT);
    pinMode(_q2, INPUT);
    pinMode(_q3, INPUT);
    pinMode(_q4, INPUT);
    digitalWrite(_q1, 1);
    digitalWrite(_q4, 0);
    pinMode(_q1, OUTPUT);
    pinMode(_q4, OUTPUT);
}

void Ringer::set_coasting()
{
    pinMode(_q1, INPUT);
    pinMode(_q2, INPUT);
    pinMode(_q3, INPUT);
    pinMode(_q4, INPUT);
}
