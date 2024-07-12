#include <Arduino.h>
#include "callerid.h"
#include "config.h"

#define TX_BUF_LEN (64)
void transmit_caller_id(const char *number)
{
    Serial.print("Transmitting Caller ID: ");
    Serial.println(number);
    char txbuf[TX_BUF_LEN];
    memset(txbuf, 0, TX_BUF_LEN);

    char message[] = {0x30, 0x39, 0x33, 0x30, 0x31, 0x32, 0x32, 0x34, 0x36, 0x30, 0x39, 0x35, 0x35, 0x35, 0x31, 0x32, 0x31, 0x32, 0x00};
    char msglen = strlen(message);

    txbuf[0] = 0x04; // message type
    txbuf[1] = msglen;

    memcpy(txbuf + 2, message, msglen);
    int checksum_position = msglen + 2;

    txbuf[checksum_position] = modulo256(txbuf, checksum_position);

    for (int i = 0; i < 30; i++)
    {
        // modem.write(0x55);
    }

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