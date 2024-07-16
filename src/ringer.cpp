#include <Arduino.h>
#include "ringer.h"

Ringer::Ringer(int a1, int a2, int en, int relay, int freq)
{
    _a1 = a1;
    _a2 = a2;
    _en = en;
    _relay = relay;
    period = 1000 / freq;
    digitalWrite(_relay, 0);
    pinMode(_relay, OUTPUT);
    pinMode(_a1, OUTPUT);
    pinMode(_a2, OUTPUT);
    pinMode(_en, OUTPUT);
    set_coasting();
}

void Ringer::handle()
{
    if (millis() - last_change > period)
    {
        last_change = millis();
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

void Ringer::start()
{
    stop_at = 0;
    forward = true;
    running = true;
    last_change = 0;
    ring_counter = 0;
    set_forward();
}

void Ringer::start(int duration, std::function<void()> timeout_callback)
{
    this->timeout_callback = timeout_callback;
    stop_at = millis() + duration;
    digitalWrite(_relay, HIGH);
    start();
}

void Ringer::stop()
{
    set_coasting();
    digitalWrite(_relay, LOW);
    running = false;
}

void Ringer::set_forward()
{
    digitalWrite(_a1, 1);
    digitalWrite(_a2, 0);
    digitalWrite(_en, 1);
}

void Ringer::set_reverse()
{
    digitalWrite(_a1, 0);
    digitalWrite(_a2, 1);
    digitalWrite(_en, 1);
}

void Ringer::set_coasting()
{
    digitalWrite(_en, 0);
    digitalWrite(_a1, 0);
    digitalWrite(_a2, 0);
}
