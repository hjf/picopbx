#include <Arduino.h>
#include "callerid.h"
#include "config.h"

#define TX_BUF_LEN (64)

void send_bytes(char *message, int length);
inline void send_mark();
inline void send_space();

void transmit_caller_id(const char *number)
{
    // Serial.print("Transmitting Caller ID: ");
    // Serial.println(number);
    char txbuf[TX_BUF_LEN];
    memset(txbuf, 0, TX_BUF_LEN);

    char message[] = {0x30, 0x39, 0x33, 0x30, 0x31, 0x32, 0x32, 0x34, 0x36, 0x30, 0x39, 0x35, 0x35, 0x35, 0x31, 0x32, 0x31, 0x32, 0x00};
    char msglen = strlen(message);

    txbuf[0] = 0x04; // message type
    txbuf[1] = msglen;

    memcpy(txbuf + 2, message, msglen);
    int checksum_position = msglen + 2;

    txbuf[checksum_position] = modulo256(txbuf, checksum_position);

    char c = 0x55;
    for (int j = 0; j < 8; j++)
    {
        if (c & 0x80)
            send_mark();
        else
            send_space();
        c <<= 1;
    }

    // char preamble[256];
    // memset(preamble, 0xff, 38);
    // memset(preamble + 39, 0xff, 22);
    // disable_start_stop_bits(true);
    // disable_start_stop_bits(false);
    // bell202_send(txbuf, checksum_position + 1, true);

    // modem.sendTone();
    // modem.write((uint8_t *)txbuf, strlen(txbuf));
    // Serial.print("Transmitting Caller ID: ");
    // for (int i = 0; i < checksum_position + 1; i++)
    // {
    //     Serial.print(txbuf[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();
}

uint8_t modulo256(char *buffer, int len)
{
    uint8_t checksum = 0;

    for (int i = 0; i < len; i++)
    {
        checksum += buffer[i];
    }
    return (~checksum) + 1;
}

// at 1200 baud, 1 bit is 833us
#define HALF_BIT_TIME_MARK 417
// at 2200baud, 1 bit is 454us
#define HALF_BIT_TIME_SPACE 227

void send_bytes(char *message, int length)
{
    pinMode(BELL202_PIN, OUTPUT);
    digitalWrite(BELL202_PIN, LOW);
    delay(1);

    // first the seizure command, 32 bytes of 0x55
    for (int i = 0; i < 32 * 4; i++)
    {
        send_space();
        send_mark();
    }
    // then the sync signal, 21 bytes of 0xff
    for (int i = 0; i < 21 * 8; i++)
    {
        send_mark();
    }
    // finally the message:
    for (int i = 0; i < length; i++)
    {
        char c = message[i];
        // start bit
        send_space();
        // 8 data bits
        for (int j = 0; j < 8; j++)
        {
            if (c & 0x01)
                send_mark();
            else
                send_space();
            c >>= 1;
        }
        // stop bit
        send_mark();
    }
    // postamble
    send_mark();
    delay(1);
    pinMode(BELL202_PIN, INPUT);
}

inline void send_mark()
{
    digitalWrite(BELL202_PIN, HIGH);
    delayMicroseconds(HALF_BIT_TIME_MARK);
    digitalWrite(BELL202_PIN, LOW);
    delayMicroseconds(HALF_BIT_TIME_MARK);
}

inline void send_space()
{
    digitalWrite(BELL202_PIN, HIGH);
    delayMicroseconds(HALF_BIT_TIME_SPACE);
    digitalWrite(BELL202_PIN, LOW);
    delayMicroseconds(HALF_BIT_TIME_SPACE);
}