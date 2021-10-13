#ifndef transmit_ir_h
#define transmit_ir_h

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

byte timerPwmPin = 14;
IRsend irsend(timerPwmPin);

// uint16_t rawData[67] = {9004, 4518, 564, 568, 564, 566, 564, 568, 564, 568, 564, 568, 566, 566, 566, 566, 564, 574, 564, 568, 566, 568, 564, 568, 564, 568, 564, 1712, 564, 1714, 562, 566, 566, 572, 566, 1712, 564, 1712, 564, 1712, 562, 1714, 564, 566, 566, 566, 564, 568, 564, 572, 566, 568, 564, 568, 564, 568, 564, 568, 564, 1710, 566, 1712, 564, 1710, 564, 1710, 566};

uint16_t rawData[67];

void initTx()
{
    irsend.begin();
}

/*This function takes a String with IR Signal & Transmit the IR Signal*/
void transmitTX(String data)
{
    // // TODO: getsize of rawData in a Dynamic way.
    // uint8_t i = 0, j = 0;
    // while (j < data.length())
    // {
    //     if (data.charAt(j) == ',')
    //     {
    //         i++;
    //     }
    //     else
    //     {
    //         rawData[i] = data.charAt(j);
    //     }
    //     j++;
    // }
    // irsend.sendNEC(0xCF00F);
    // irsend.sendRaw(rawData, 67, 38);
    // Serial.println("rawData size = " + String(i + 1));
    irsend.sendRaw(rawData, 67, 38);
    delay(30);
    Serial.println("RAWDATA has been sent . . .");
}

#endif