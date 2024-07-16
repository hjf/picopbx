#include <Arduino.h>
#include "callerid.h"
#include "config.h"

#define TX_BUF_LEN (64)
// at 1200 baud, 1 bit is 833us
#define HALF_BIT_TIME_MARK (417)
// at 2200baud, 1 bit is 454us
#define HALF_BIT_TIME_SPACE (227)

int CallerId::transmit_caller_id(const char *message, size_t length)
{
    Serial.print("Transmitting Caller ID: ");
    if (length > TX_BUF_LEN - 2)
    {
        Serial.println("ERROR Message too long");
        return -1;
    }

    pinMode(BELL202_PIN, OUTPUT);
    digitalWrite(BELL202_PIN, LOW);
    Serial.println(message);
    char txbuf[TX_BUF_LEN];
    memset(txbuf, 0, TX_BUF_LEN);
    memcpy(txbuf + 2, message, length);

    txbuf[0] = 0x04; // message type
    txbuf[1] = length;

    memcpy(txbuf + 2, message, length);
    int checksum_position = length + 2;

    txbuf[checksum_position] = modulo256(txbuf, checksum_position);
    delay(1);
    send_bytes(txbuf, checksum_position + 1);
    delay(1);

    pinMode(BELL202_PIN, INPUT);
    return 0;
}

uint8_t CallerId::modulo256(char *buffer, int len)
{
    uint8_t checksum = 0;

    for (int i = 0; i < len; i++)
    {
        checksum += buffer[i];
    }
    return (~checksum) + 1;
}

void CallerId::send_bytes(char *message, int length)
{

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
}

inline void CallerId::send_mark()
{
    digitalWrite(BELL202_PIN, HIGH);
    delayMicroseconds(HALF_BIT_TIME_MARK);
    digitalWrite(BELL202_PIN, LOW);
    delayMicroseconds(HALF_BIT_TIME_MARK);
}

inline void CallerId::send_space()
{
    digitalWrite(BELL202_PIN, HIGH);
    delayMicroseconds(HALF_BIT_TIME_SPACE);
    digitalWrite(BELL202_PIN, LOW);
    delayMicroseconds(HALF_BIT_TIME_SPACE);
    digitalWrite(BELL202_PIN, HIGH);
    delayMicroseconds(HALF_BIT_TIME_SPACE);
    digitalWrite(BELL202_PIN, LOW);
    delayMicroseconds(152);
}