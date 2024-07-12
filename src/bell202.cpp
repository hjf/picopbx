#include <Arduino.h>
#include <PWMAudio.h>
#include "config.h"
#include "bell202.h"
#include "bell202_samples.h"

const int16_t *mark_start = (const int16_t *)mark_samples;
const int16_t *mark_p = mark_start;

const int16_t *space_start = (const int16_t *)space_samples;
const int16_t *space_p = space_start;

PWMAudio bell_pwm(BELL202_PIN);

int message_position = 0;
int bit_position = 0;
int tx_length;
bool tone_mode = false;
bool tone_mark = false;

char message[256];

void bell202_callback()
{
    while (bell_pwm.availableForWrite() >= FSK_SAMPLES_PER_BIT)
    {
        if (!message_position && !tone_mode)
            return;

        char current_character = message[tx_length - message_position];
        int is_mark;
        if (bit_position == 0 || (tone_mode && !tone_mark))
        {
            // start bit
            is_mark = 0;
        }
        else if (bit_position == 9 || (tone_mode && tone_mark))
        {
            // stop bit
            is_mark = 1;
        }
        else
        {
            is_mark = (current_character >> (bit_position - 1)) & 0x01;
        }
        bit_position++;

        if (tone_mode)
        {
            bit_position = 0;
            message_position = 0;
        }

        int count = 0;

        const int16_t *p = is_mark ? mark_p : space_p;
        const int16_t *start = is_mark ? mark_start : space_start;

        while (count < FSK_SAMPLES_PER_BIT)
        {
            auto sample = *p++;
            bell_pwm.write(sample);
            count += 2;
        }

        if (bit_position >= 10)
        {
            bit_position = 0;
            message_position--;
        }
    }
}

void bell202_begin()
{
    bell_pwm.onTransmit(bell202_callback);
    bell_pwm.begin(FSK_SAMPLING_RATE);
}

void bell202_stop()
{
    bell_pwm.flush();
    bell_pwm.end();
}

void wait_for_message()
{
    while (message_position)
    {
        yield();
    }
}
int bell202_send(const char *msg, size_t len, bool blocking)
{
    if (len > sizeof(message))
    {
        return -1;
    }
    wait_for_message();

    memset(message, 0, sizeof(message));
    tx_length = len;
    message_position = len;
    bit_position = 0;
    if (blocking)
    {
        while (message_position)
        {
            yield();
        }
    }
    return 0;
}

void bell202_tone(bool mark, int duration)
{
    wait_for_message();
    unsigned long endtime = millis() + duration;

    tone_mark = mark;
    tone_mode = true;

    while (millis() < endtime)
    {
        yield();
    }
}
