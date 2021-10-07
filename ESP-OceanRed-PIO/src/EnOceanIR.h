#ifndef enocean_ir_h
#define enocean_ir_h

#include <Arduino.h>
// #include <EnOcean.h>
#include <TransmitIR.h>

byte learnedSensor[] = {0xFE, 0xFA, 0x27, 0xC5};

boolean sendFlag = false;

void sendIROnCommand(byte senderID[], byte status)
{
    // //     lookupEnOceanPacket();
    for (int i = 0; i < 4; i++)
    {
        // Serial.print(recivedTelegram.senderID[i], HEX);
        // Serial.print(senderID[i], HEX);
        if (senderID[i] != learnedSensor[i])
        {
            Serial.println("ID DOES NOT MATCH LEARND SENSOR! ");
            Serial.print("Learned Sensor = ");
            Serial.print(learnedSensor[0], HEX);
            Serial.print(", ");
            Serial.print(learnedSensor[1], HEX);
            Serial.print(", ");
            Serial.print(learnedSensor[2], HEX);
            Serial.print(", ");
            Serial.println(learnedSensor[3], HEX);
            Serial.print("SenderID = ");
            Serial.print(senderID[0], HEX);
            Serial.print(", ");
            Serial.print(senderID[1], HEX);
            Serial.print(", ");
            Serial.print(senderID[2], HEX);
            Serial.print(", ");
            Serial.print(senderID[3], HEX);
            break;
        }
    }
    if (status == 0x20)
    {
        //         setBSend(true);
        //         transmitTX();
        // Send IR SIGNAL
        // irsend.sendRaw(rawData, 67, 38);
        transmitTX();
    }
}

#endif