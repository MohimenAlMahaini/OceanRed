/*
* This class is responable for EnOcean signal
* Implementation based on EnOcean Serial Protocol 3
* Author: Mohimen Al Mahaini
*/
#ifndef enocean_h
#define enocean_h

#include <Arduino.h>
#include <CRC8.h>
#include <EnOceanIR.h>

#define TCM_BAUD_RATE 57600
#define ESP_UART1_TX 4
#define ESP_UART1_RX 35
#define MAX_PAYLOAD 200

// boolean recived = false;

// Global Variables
int rxBuffer[256]; 
struct EnOcean
{
    byte syncByte = 0x55;
    int dataLength;
    byte optionalLength;
    byte packetType;
    byte CRC8H;
    byte data[256];
    byte optionalData[256];
    byte CRC8D;
} telegram;

struct EnOceanPacket
{
    byte packetType;
    byte senderID[4];
    byte payload[MAX_PAYLOAD];
    byte status;
} recivedTelegram;

boolean startEnOceanTeachIn = false;
String enOceanArray[20];
uint index_ptr_enocean = 0;
uint sizeOfEnoceanArray = sizeof(enOceanArray) / sizeof(enOceanArray[0]);

void setupUART()
{
    // Serial1.begin(baud-rate, protocol, RX pin, TX pin);
    Serial1.begin(TCM_BAUD_RATE, SERIAL_8N1, ESP_UART1_RX, ESP_UART1_TX);
    Serial.println("Serial Txd is on pin: " + String(ESP_UART1_TX));
    Serial.println("Serial Rxd is on pin: " + String(ESP_UART1_RX));
    Serial1.flush();
}

/*This finction takes a byte array & the size of the array as an argument and returns the calculated CRC8 for the array*/
byte calcCrC8(byte data[], int sizeOfArray)
{
    int u8CRC = 0;
    for (int i = 0; i < sizeOfArray; i++)
    {
        u8CRC = proccrc8(u8CRC, data[i]);
    }
    return u8CRC;
}

/*This function displays the recived EnOcean signal (for debug / clarification)*/
void displayRecivedTelegram(byte header[])
{
    Serial.println("########ROW DATA########");
    Serial.print("SyncByte = ");
    Serial.println(telegram.syncByte);
    Serial.printf("Hader =");
    for (int i = 0; i < sizeof(header); i++)
    {
        Serial.print((header[i]), HEX);
    }
    Serial.println("");
    Serial.print("CRC8H = ");
    Serial.println((telegram.CRC8H), HEX);
    Serial.printf("DATA = ");
    for (int j = 0; j < telegram.dataLength; j++)
    {
        Serial.print("0x");
        Serial.print((telegram.data[j]), HEX);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.printf("OPTIONAL DATA = ");
    for (int jj = 0; jj < telegram.optionalLength; jj++)
    {
        Serial.print("0x");
        Serial.print((telegram.optionalData[jj]), HEX);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.printf("CRC8D = %02X\n", telegram.CRC8D);
    Serial.println("#####END OF RAW DATA#####");
}

/*This is a recursive function it Reads Serial1 with availabilty check and saves the read values to rxBuffer[]*/
void readAvailableSerial(int enOceanbyte)
{
    if (Serial1.available() >= enOceanbyte)
    {
        for (int i = 0; i < enOceanbyte; i++)
        {
            rxBuffer[i] = Serial1.read();
        }
    }
    else
    {
        delay(100);
        readAvailableSerial(enOceanbyte);
    }
}

/* This function Validates the recived EnOcean Signal
*  Validate if the telegram came from EnOCean Rocker or FTKE.
*  return boolean
*/
boolean isTelegramValid()
{
    if (telegram.packetType != 0x01) // RADIO_ERP1
    {
        return false;
    }

    if (telegram.data[0] != 0xF6 /*|| telegram.data[0] != 0xD5*/) // Telegram Type RORG see EEP page 14 (1BS == D5 implement latter)
    {
        return false;
    }
    return true;
}

/**
 * This Function stores the recived EnOcean Telegrams in enoceanArray.
 */
void saveToArray()
{
    if (index_ptr_enocean == sizeOfEnoceanArray)
    {
        index_ptr_enocean = 0;
    }

    String arryString = "";
    String hexString;

    if (telegram.data[1] == 0xf0 || telegram.data[1] == 0xe0) // ftke
    {
        arryString += "FTKE";
    }
    else if (telegram.data[1] == 0x00 || telegram.data[1] == 0x10 || telegram.data[1] == 0x20 || telegram.data[1] == 0x30 || telegram.data[1] == 0x40 || telegram.data[1] == 0x50 || telegram.data[1] == 0x60 || telegram.data[1] == 0x70)
    {
        arryString += "EnOcean Rocker";
    }

    arryString += "#";

    // add SenderID to oceanString
    for (int i = 0; i < 4; i++)
    {
        hexString += String(recivedTelegram.senderID[i], HEX);
        if (recivedTelegram.senderID[i] == 0)
        {
            hexString += "0";
        }
    }
    Serial.print("HEXSTRING = ");
    Serial.println(hexString);
    arryString += "0x";
    arryString += hexString;
    // arrayString = <type>#0x<senderId>
    arryString += "*"; //GET PAYLOAD FOR 1BS
    hexString = String(telegram.data[1], HEX);
    if (telegram.data[1] == 0)
    {
        hexString += "0";
    }
    arryString += "0x";
    arryString += hexString;
    enOceanArray[index_ptr_enocean] = arryString;
    index_ptr_enocean++;
}

void getEnOceanSenderInfo()
{
    recivedTelegram.packetType = telegram.packetType; //get PacketType
    byte temp[4];
    // Get Sender ID
    for (int i = 0; i < 4; i++)
    {
        temp[i] = telegram.data[telegram.dataLength - 2 - i]; // Recive SenderID last in first out
    }
    // reverse receivedTelegram (senderId Correct order)
    for (int y = 0; y < 4; y++)
    {
        recivedTelegram.senderID[y] = temp[3 - y];
    }
    // Get Payload (User Data)
    for (int ii = 0; ii < telegram.dataLength - 5; ii++)
    {
        recivedTelegram.payload[ii] = telegram.data[ii + 1];
    }
    // Code for debuging
    // Serial.println("Data has been parsed");
    // for (int yy = 0; yy < 4; yy++) // for debug
    // {
    //     Serial.print(" 0x");
    //     Serial.print(recivedTelegram.senderID[yy], HEX);
    // }
    // Serial.println("");

    recivedTelegram.status = telegram.data[telegram.dataLength - 1];

    // Check if listening & search for the signal
    if (isTelegramValid())
    {
        compareRxWithRAMEntries(recivedTelegram.senderID, telegram.data[1]);
        if (startEnOceanTeachIn)
        {
            saveToArray();
        }
    }
}

/*This function Reads Serial1 (TCM) and Recive EnOcean Telegrams*/
void rxEnOcean()
{
    readAvailableSerial(6);
    if (rxBuffer[0] == telegram.syncByte)
    {
        // Get Header
        telegram.dataLength = (rxBuffer[1] * 256) + rxBuffer[2];
        telegram.optionalLength = rxBuffer[3];
        telegram.packetType = rxBuffer[4];
        telegram.CRC8H = rxBuffer[5];
        // validate CRC8H  //TODO → create new function for validating CRC8
        byte headerData[] = {rxBuffer[1], rxBuffer[2], rxBuffer[3], rxBuffer[4]};
        if (telegram.CRC8H != calcCrC8(headerData, sizeof(headerData)))
        {
            Serial.println("Recived CRC8h != Calculated CRC8h"); // Something went wrong with transmition. Start over
        }
        else
        {
            // Serial.println("Recived CRC8h == Calculated CRC8h  "); // All good, proceed to recive data.
            // Read Data
            readAvailableSerial(telegram.dataLength);
            // Get Data
            for (int i = 0; i < telegram.dataLength; i++)
            {
                telegram.data[i] = rxBuffer[i]; // copy rxBuffer to data
            }
            // Read Optional Data
            readAvailableSerial(telegram.optionalLength);
            // Get Optional Data
            for (int ii = 0; ii < telegram.optionalLength; ii++)
            {
                telegram.optionalData[ii] = rxBuffer[ii];
            }
            // Calculate CRC8D
            readAvailableSerial(1);
            telegram.CRC8D = rxBuffer[0];
            byte data[telegram.dataLength + telegram.optionalLength];
            for (int j = 0; j < telegram.dataLength; j++)
            {
                data[j] = telegram.data[j];
            }
            for (int jj = 0; jj < telegram.optionalLength; jj++)
            {
                data[telegram.dataLength + jj] = telegram.optionalData[jj];
            }
            if (calcCrC8(data, sizeof(data)) != telegram.CRC8D)
            {
                Serial.println("Recived CRC8D != Calculated CRC8D"); // Something went wrong with transmition. Start over
                Serial.print(calcCrC8(data, sizeof(data)), HEX);
                Serial.print(" != ");
                Serial.println(telegram.CRC8D, HEX);
                for (int ij = 0; ij < sizeof(data); ij++)
                {
                    Serial.print(data[ij], HEX);
                }
                Serial.println(" ");
            }
            else
            {
                // Serial.println("Recived CRC8D == Calculated CRC8D"); // All good, proceed.
                getEnOceanSenderInfo();
            }
            // displayRecivedTelegram(headerData);
        }
    }
}

void vEnOceanTask(void *pvParameters)
{
    for (;;)
    {
        rxEnOcean();
        // taskYIELD();
        // sendIROnCommand(recivedTelegram.senderID, recivedTelegram.status);
    }
}

#endif
/**
 *  PROG | DATA | Sender ID | Status
 *      f6 | e0 | fe fa 27 c5 | 20
 *      f6  e0  fe fa 27 c5 20
 *      f6 20 fe ff fe 63 30
 *      f6 00 fe ff fe 63 20
*/