#ifndef transmit_ir_h
#define transmit_ir_h

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

byte timerPwmPin = 14;
IRsend irsend(timerPwmPin);

// uint16_t rawData[67] = {9004, 4518, 564, 568, 564, 566, 564, 568, 564, 568, 564, 568, 566, 566, 566, 566, 564, 574, 564, 568, 566, 568, 564, 568, 564, 568, 564, 1712, 564, 1714, 562, 566, 566, 572, 566, 1712, 564, 1712, 564, 1712, 562, 1714, 564, 566, 566, 566, 564, 568, 564, 572, 566, 568, 564, 568, 564, 568, 564, 568, 564, 1710, 566, 1712, 564, 1710, 564, 1710, 566};
// uint16_t rawData[67];

void initTx()
{
    irsend.begin();
}

int calculateBufferSize(String data)
{
    uint i = 0, j = 0;
    while (j < data.length())
    {
        if (data.charAt(j) == ',')
        {
            i++;
        }
        j++;
    }
    return i + 1;
}

/*This function takes a String with IR Signal & Transmit the IR Signal*/
void transmitTX(String data)
{
    Serial.println("DATA = " + data);
    uint bufferSize = calculateBufferSize(data);
    uint16_t *rawData = new uint16_t[bufferSize]; // Create Array in Heap

    // fill rawData
    char *pch;
    char str[data.length() + 1];              // Prepare the character array (the buffer) , Length (with one extra character for the null terminator)
    data.toCharArray(str, data.length() + 1); // Copy String to charArray

    pch = strtok(str, ",");
    int j = 0;
    while (pch != NULL)
    {
        rawData[j] = atoi(pch);
        pch = strtok(NULL, ",");
        j++;
    }
    // Send IR Signal
    Serial.println();
    irsend.sendRaw(rawData, bufferSize, 38);

    Serial.println("RAWDATA has been sent . . .");
    delete[] rawData; // clean memory up
}

#endif
