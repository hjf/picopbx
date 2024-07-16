#include <Arduino.h>
#include "util.h"
uint8_t modulo256(char *buffer, int len)
{
    uint8_t checksum = 0;

    for (int i = 0; i < len; i++)
    {
        checksum += buffer[i];
    }
    return (~checksum) + 1;
}

char findMajority(char arr[], char n)
{
    int maxCount = 0;
    int index = -1; // sentinels
    for (int i = 0; i < n; i++)
    {
        int count = 0;
        for (int j = 0; j < n; j++)
        {
            if (arr[i] == arr[j])
                count++;
        }

        // update maxCount if count of
        // current element is greater
        if (count > maxCount)
        {
            maxCount = count;
            index = i;
        }
    }

    // if maxCount is greater than n/2
    // return the corresponding element
    if (maxCount > n / 2)
        return arr[index];

    return 0;
}